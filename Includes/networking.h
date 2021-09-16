#ifndef __NETWORKING_H
#define __NETWORKING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip/netif.h>

#define ERR_NO_INTERFACE 1
#define ERR_NO_DHCP_RESPONSE 2

int setupNetwork(void* DHCP);

void closeNetwork();

#ifdef __cplusplus
}
#endif
#endif
