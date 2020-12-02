#include <lwip/dhcp.h>
#include <lwip/dhcp6.h>
#include <lwip/init.h>
#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>
#include <pktdrv.h>
#include <ctime>

#include "outputLine.h"
#include "networking.h"

#define PKT_TMR_INTERVAL 1 /* ms */

extern "C" {
  struct netif nforce_netif, *g_pnetif;
  err_t nforceif_init(struct netif *netif);
}

void http_server_netconn_init(void);
static void packet_timer(void *arg);

static void tcpip_init_done(void *arg)
{
  sys_sem_t *init_complete = static_cast<sys_sem_t*>(arg);
  sys_sem_signal(init_complete);
}

static void packet_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  Pktdrv_ReceivePackets();
  sys_timeout(PKT_TMR_INTERVAL, packet_timer, NULL);
}

int setupNetwork(void* DHCP) {
  bool dhcp = *static_cast<bool*>(DHCP);

  static ip4_addr_t ipaddr, netmask, gw;
  sys_sem_t init_complete;

  if (dhcp) {
    IP4_ADDR(&gw, 0,0,0,0);
    IP4_ADDR(&ipaddr, 0,0,0,0);
    IP4_ADDR(&netmask, 0,0,0,0);
  } else {
    IP4_ADDR(&gw, 10,0,1,1);
    IP4_ADDR(&ipaddr, 10,0,1,7);
    IP4_ADDR(&netmask, 255,255,255,0);
  }
  /* Initialize the TCP/IP stack. Wait for completion. */
  sys_sem_new(&init_complete, 0);
  tcpip_init(tcpip_init_done, &init_complete);
  sys_sem_wait(&init_complete);
  sys_sem_free(&init_complete);

  g_pnetif = netif_add(&nforce_netif, &ipaddr, &netmask, &gw,
                       NULL, nforceif_init, ethernet_input);
  if (!g_pnetif) {
    return 1;
  }
  netif_set_default(g_pnetif);
  netif_set_up(g_pnetif);

  if (dhcp) {
    dhcp6_enable_stateless(g_pnetif);
    dhcp_start(g_pnetif);
  }

  packet_timer(NULL);

  if (dhcp) {
    time_t start = time(NULL);
    while (dhcp_supplied_address(g_pnetif) == 0) {
      if((time(NULL) - start) <= 7) {
        NtYieldExecution();
      }
      else {
        outputLine("Couldn't get DHCP settings!\n");
        break;
      }
    }
  }
  return 0;
}

void closeNetwork() {
  Pktdrv_Quit();
}
