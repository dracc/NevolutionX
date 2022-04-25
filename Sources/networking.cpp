// clang-format off
// The lwip headers pull in fflush in a way that conflicts with STL use in the InfoLog class
// and must be included last to prevent compilation errors.
#include "infoLog.hpp"
// clang-format on

#include "networking.hpp"
#include <nxdk/net.h>
#include <ctime>


extern "C" {
extern struct netif* g_pnetif;
}

int setupNetwork(bool dhcp, staticIP static_ip_info) {
  nx_net_parameters_t params;
  if (!dhcp) {
    params.ipv4_mode = NX_NET_STATIC;
    params.ipv4_ip = static_ip_info.ip.addr;
    params.ipv4_gateway = static_ip_info.gateway.addr;
    params.ipv4_netmask = static_ip_info.netmask.addr;
    params.ipv4_dns1 = static_ip_info.dns1.addr;
    params.ipv4_dns2 = static_ip_info.dns2.addr;
  } else if (params.ipv4_dns1 != 0x00000000 || params.ipv4_dns2 != 0x00000000) {
    params.ipv4_mode = NX_NET_DHCP;
    params.ipv4_dns1 = static_ip_info.dns1.addr;
    params.ipv4_dns2 = static_ip_info.dns2.addr;
  }
  return nxNetInit(&params);
}

void closeNetwork() {
  // nxNetShutdown();
}
