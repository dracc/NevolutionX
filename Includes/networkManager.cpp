#include "networkManager.h"
#include <utility>

#ifdef NXDK

#include "networking.h"

extern "C" {
extern struct netif* g_pnetif;
}

#endif

NetworkManager::NetworkManager(Config config) :
    config(std::move(config)), status(INIT_NONE) {
}

void NetworkManager::asyncInit() {
  assert(status == INIT_NONE);
  init_thread = std::thread(thread_main, this);
}

void NetworkManager::thread_main(NetworkManager* manager) {
#ifdef NXDK
  const netConfig& net = manager->config.settings.net;

  staticIP static_ip;
  static_ip.gateway.addr = net.getStaticGateway();
  static_ip.ip.addr = net.getStaticIP();
  static_ip.netmask.addr = net.getStaticNetmask();

  int status = setupNetwork(net.getUseDHCP(), static_ip);
  if (status) {
    manager->status = INIT_FAILED;
  } else {
    manager->status = INIT_SUCCEEDED;
  }

#else
  manager->status = INIT_FAILED;
#endif
}

std::string NetworkManager::IPAddressString() const {
  NetworkStatus current_status = status;
  if (current_status != INIT_SUCCEEDED && current_status != RUNNING) {
    return std::string();
  }

  if (!g_pnetif) {
    return std::string();
  }

  return ip4addr_ntoa(netif_ip4_addr(g_pnetif));
}
