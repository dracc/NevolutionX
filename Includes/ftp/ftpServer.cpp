#include <iostream>
#include <iomanip>
#include <algorithm>
#include "outputLine.h"
#include "ftpServer.h"

#ifdef NXDK
#include <lwip/opt.h>
#include <lwip/arch.h>
#include <lwip/errno.h>
#include <lwip/debug.h>
#include <lwip/dhcp.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>
#include <pktdrv.h>
#include <nxdk/mount.h>
#include <xboxkrnl/xboxkrnl.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif


// get sockaddr, IPv4 or IPv6:
void* ftpServer::getInAddr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#ifdef NXDK
const char* gai_strerror(int errc) {
  switch (errc) {
  case 200:
    return "EAI_NONAME";
  case 201:
    return "EAI_SERVICE";
  case 202:
    return "EAI_FAIL";
  case 203:
    return "EAI_MEMORY";
  case 204:
    return "EAI_FAMILY";
  case 210:
    return "HOST_NOT_FOUND";
  case 211:
    return "NO_DATA";
  case 212:
    return "NO_RECOVERY";
  case 213:
    return "TRY_AGAIN";
  default:
    return "ERR_OK";
  }
}
#endif

std::string sock_strerror(int errc) {
  switch (errc) {
  case EACCES:
    return "EACCES";
  case EAFNOSUPPORT:
    return "EAFNOSUPPORT";
  case EINVAL:
    return "EINVAL";
  case ENFILE:
    return "ENFILE";
  case EMFILE:
    return "EMFILE";
  case ENOBUFS:
    return "ENOBUFS";
  case ENOMEM:
    return "ENOMEM";
  case EPROTONOSUPPORT:
    return "EPROTONOSUPPORT";
  default:
    return "ERR_OK";
  }
}

ftpServer::ftpServer(int port) :
  _port(port) {
  FD_ZERO(&master);    // clear the master and temp sets
  FD_ZERO(&readFds);

  // Set hints for our future socket(s)
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
}

int ftpServer::init(void) {
  int yes = 1;
  if ((i = getaddrinfo(NULL, std::to_string(_port).c_str(), &hints, &ai)) != 0) {
    outputLine("Error: selectserver\n");
    return 5;
  }

  /* Get network settings */
  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }
    break;
  }

  freeaddrinfo(ai);

  if (p == NULL) {
    outputLine("Error: selectServer: failed to bind\n");
    return 1;
  }

  /* Start listening */
  if (listen(listener, 10) == -1) {
    outputLine("Error: Listen\n");
    return 2;
  }
  FD_SET(listener, &master);
  fdmax = listener;
  return 0;
}

int ftpServer::run()
{
  /* `select` a file descriptor */
  for (;;) {
    readFds = master;
    if (select(fdmax+1, &readFds, NULL, NULL, NULL) == -1) {
      outputLine("Error: Select\n");
      return 3;
    }
    for(i = 0; i <= fdmax; ++i) {
      if (FD_ISSET(i, &readFds)) { // we got one!!
        if (i == listener) {
          // handle new connections
          addrlen = sizeof raddr;
          newfd = accept(listener,
                         (struct sockaddr *)&raddr,
                         &addrlen);

          if (newfd == -1) {
            outputLine("Error: accept: %s\n", sock_strerror(errno).c_str());
          } else {
            FD_SET(newfd, &master); // add to master set
            if (newfd > fdmax) {    // keep track of the max
              fdmax = newfd;
            }
            clients[newfd] = new ftpConnection(newfd, this);
          }
        } else {
          if (!clients[i]->update()) {
            forgetConnection(i);
          }
        }
      }
    }
  }
}

void ftpServer::forgetConnection(int fd) {
  delete(clients[fd]);
  clients.erase(fd);
  FD_CLR(fd, &master);
  outputLine("Closing %d!\n", fd);
  close(fd); // bye!
}

int ftpServer::openConnection(std::string const& addr, std::string const& port) {
  int ret;
  int rv;
  if ((rv = getaddrinfo(addr.c_str(), port.c_str(), &hints, &ai)) == 0)
  {
    if ((ret = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) != -1)
    {
      if (connect(ret, ai->ai_addr, ai->ai_addrlen) != 0)
      {
        outputLine("Connecting socket %d failed; %s\n", ret, sock_strerror(errno).c_str());
        close(ret);
        ret = -1;
      }
    } else {
      outputLine("Socket creation failed; %s\n", sock_strerror(errno).c_str());
    }
  } else {
    outputLine("Getting address info failed; %s\n", gai_strerror(rv));
    ret = -1;
  }
  freeaddrinfo(ai);
  return ret;
}

int thread_runner(void* server) {
  ftpServer* s = static_cast<ftpServer*>(server);
  return s->run();
}
