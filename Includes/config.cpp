#include "config.hpp"

#include <fstream>

#ifdef NXDK
#define SEPARATOR "\\"
#define HOME "D:" SEPARATOR
#else
#define SEPARATOR "/"
#define HOME "." SEPARATOR
#endif

Config::Config() {
  std::ifstream configFile(HOME "config.json");
  configFile >> this->json;
}
