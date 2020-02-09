#ifndef __NETWORKING_H
#define __NETWORKING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip/netif.h>

int setupNetwork(void* DHCP);

void closeNetwork();

#ifdef __cplusplus
}
#endif
#endif
