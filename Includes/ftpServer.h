#ifndef __FTPSERVER_H
#define __FTPSERVER_H

#include <thread>

#include <string>
#include <map>
#include "ftpConnection.h"
#include "config.hpp"
#ifdef NXDK
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#else
#include <netdb.h>
#endif

class ftpServer {
  fd_set master;
  fd_set readFds;
  int fdmax;

  std::map<int, ftpConnection*> clients;

  int listener;
  int newfd;
  int i;

  struct sockaddr_storage raddr;
  socklen_t addrlen;
  struct addrinfo hints, *ai, *p;

  std::thread serverThread;

  void* getInAddr(struct sockaddr *sa);
  static int thread_runner(ftpServer *server);

public:
  ftpServer(ftpConfig const* conf);
  int init();
  int run();
  void runAsync();
  void forgetConnection(int fd);
  int openConnection(std::string const& addr, std::string const& port);

  const ftpConfig *conf;
};

#endif
