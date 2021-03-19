#include "wipeCache.hpp"
#include <array>
#include <string>

#ifdef NXDK
#include <nxdk/format.h>
#endif

bool wipe_cache() {
#ifdef NXDK
  std::array<std::string, 3> partitions = {
    "\\Device\\Harddisk0\\Partition3", // "X"
    "\\Device\\Harddisk0\\Partition4", // "Y"
    "\\Device\\Harddisk0\\Partition5" // "Z"
  };
  for (std::string s: partitions) {
    if (nxFormatVolume(s.c_str(), 0) == false) {
      return false;
    }
  }
#endif
  return true;
}
