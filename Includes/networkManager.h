#ifndef NEVOLUTIONX_NETWORKMANAGER_H
#define NEVOLUTIONX_NETWORKMANAGER_H

#include <thread>
#include "config.hpp"

class NetworkManager {
public:
  typedef enum
  {
    INIT_NONE,
    INIT_SUCCEEDED,
    INIT_FAILED,
    RUNNING
  } NetworkStatus;

  explicit NetworkManager(Config config);

  // Initializes the network asynchronously.
  void asyncInit();

  inline NetworkStatus getStatus() const { return status; }
  std::string IPAddressString() const;

  inline bool isNewlyInitialized() {
    if (status == INIT_SUCCEEDED) {
      status = RUNNING;
      init_thread.join();
      return true;
    }

    return false;
  }

private:
  static void thread_main(NetworkManager* manager);

  Config config;

  std::atomic<NetworkStatus> status;
  std::thread init_thread;
};


#endif // NEVOLUTIONX_NETWORKMANAGER_H
