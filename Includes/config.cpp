#include "config.hpp"

#include <fstream>

Config::Config() {
  std::ifstream configFile("config.json");
  configFile >> this->json;
}
