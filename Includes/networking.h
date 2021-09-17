#ifndef __NETWORKING_H
#define __NETWORKING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip/netif.h>

#define ERR_NO_INTERFACE 1
#define ERR_NO_DHCP_RESPONSE 2

typedef struct _staticIP {
  ip4_addr_t gateway;
  ip4_addr_t ip;
  ip4_addr_t netmask;
} staticIP;

int setupNetwork(bool useDHCP, staticIP static_ip_info);

void closeNetwork();

#ifdef __cplusplus
}
#endif
#endif
