#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../3rdparty/json.hpp"

class Config {
public:
  Config();
//private:
  nlohmann::json json;
};

#endif
