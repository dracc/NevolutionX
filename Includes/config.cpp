#include "config.hpp"

#include <fstream>
#include <iomanip>

// Note: pulling this in can create a conflict with fflush as defined in
// <fstream> leading to a compilation failure w/ clang 12.0.0 on macOS. It is
// important that <fstream> is included before any lwip headers.
#include <lwip/inet.h>

#ifdef NXDK
#define SEPARATOR "\\"
#define HOME "A:" SEPARATOR
#else
#define SEPARATOR "/"
#define HOME "." SEPARATOR
#endif

static unsigned int parseIPV4(const std::string &val) {
  in_addr addr{0};

  if (inet_aton(val.c_str(), &addr)) {
    return addr.s_addr;
  }

  return 0;
}

static std::string ipV4String(unsigned int addr) {
  in_addr buf{0};
  buf.s_addr = addr;
  std::string ret(inet_ntoa(buf));
  return ret;
}

netConfig::netConfig() : enable(true), useDHCP(true) {
  ip4_addr_t addr;
  IP4_ADDR(&addr, 10,0,1,1);
  staticGateway = addr.addr;

  IP4_ADDR(&addr, 10,0,1,7);
  staticIP = addr.addr;

  IP4_ADDR(&addr, 255,255,255,0);
  staticNetmask = addr.addr;
}

void to_json(nlohmann::json& j, netConfig const& o) {
  j = nlohmann::json{{"enable", o.getEnabled()},
                     {"use_dhcp", o.getUseDHCP()},
                     {"static_ip", ipV4String(o.getStaticIP())},
                     {"static_gateway", ipV4String(o.getStaticGateway())},
                     {"static_netmask", ipV4String(o.getStaticNetmask())}};
}

void from_json(nlohmann::json const& j, netConfig& o) {
  if (j.contains("enable") && j["enable"].is_boolean()) {
    o.setEnabled(j["enable"]);
  }
  if (j.contains("use_dhcp") && j["use_dhcp"].is_boolean()) {
    o.setUseDHCP(j["use_dhcp"]);
  }
  if (j.contains("static_ip")) {
    o.setStaticIP(parseIPV4(j["static_ip"]));
  }
  if (j.contains("static_gateway")) {
    o.setStaticGateway(parseIPV4(j["static_gateway"]));
  }
  if (j.contains("static_netmask")) {
    o.setStaticNetmask(parseIPV4(j["static_netmask"]));
  }
}

ftpConfig::ftpConfig() {
  enable = true;
  username = "xbox";
  password = "xbox";
  port = 21;
}

void to_json(nlohmann::json& j, ftpConfig const& f) {
  j = nlohmann::json{{"enable", f.getEnabled()},
                     {"username", f.getUser()}, {"password", f.getPassword()},
                     {"port", f.getPort()}};
}

void from_json(nlohmann::json const& j, ftpConfig& f) {
  if (j.contains("enable")) {
    if (j["enable"].is_boolean()) {
      f.setEnabled(j["enable"]);
    }
  }
  if (j.contains("username")) {
    if (j["username"].is_string()) {
      f.setUser(j["username"]);
    }
  }
  if (j.contains("password")) {
    if (j["password"].is_string()) {
      f.setPassword(j["password"]);
    }
  }
  if (j.contains("port")) {
    if (j["port"].is_number()) {
      f.setPort(j["port"]);
    }
  }
}

void ftpConfig::setEnabled(bool val) {
  if (enable != val) {
    enable = val;
  }
}

void ftpConfig::setUser(std::string const& user) {
  if (username.compare(user)) {
    username = user;
  }
}

void ftpConfig::setPassword(std::string const& pwd) {
  if (password.compare(pwd)) {
    password = pwd;
  }
}

void ftpConfig::setPort(int p) {
  if (port != p) {
    port = p;
  }
}

mountConfig::mountConfig() {
  enableF = true;
  enableG = true;
}

void to_json(nlohmann::json& j, mountConfig const& o) {
  j = nlohmann::json{{"enable_f", o.getFEnabled()},
                     {"enable_g", o.getGEnabled()}};
}

void from_json(nlohmann::json const& j, mountConfig& o) {
  if (j.contains("enable_f") && j["enable_f"].is_boolean()) {
    o.setFEnabled(j["enable_f"]);
  }
  if (j.contains("enable_g") && j["enable_g"].is_boolean()) {
    o.setGEnabled(j["enable_g"]);
  }
}

void to_json(nlohmann::json& j, Settings const& o) {
  j = nlohmann::json{{"ftp", nlohmann::json(o.ftp)},
                     {"mount", nlohmann::json(o.mount)},
                     {"network", nlohmann::json(o.net)}};
}

void from_json(nlohmann::json const& j, Settings& o) {
  if (j.contains("ftp")) {
    o.ftp = j["ftp"].get<ftpConfig>();
  }
  if (j.contains("mount")) {
    o.mount = j["mount"].get<mountConfig>();
  }
  if (j.contains("network")) {
    o.net = j["network"].get<netConfig>();
  }
}

Config::Config() {
  std::ifstream configFile(HOME "config.json");
  nlohmann::json json;
  // FIXME: Once nxdk supports C++ Exceptions, this needs to be put in a try-catch block!
  configFile >> json;
  if (json.contains("settings")) {
    settings = json["settings"].get<Settings>();
  } else {
    changed = true;
  }
  if (!json.contains("menu")) {
    json["menu"] = R"([{"label": "Games", "type": "scan", "path": "F:\\Games\\"},
        {"label": "Applications", "type": "scan", "path": "F:\\Apps"},
        {"label": "Launch DVD", "type": "launch", "path": "D:\\default.xbe"},
        {"label": "Settings", "type": "settings"},
        {"label": "Reboot", "type": "reboot"}])"_json;
    changed = true;
  }
  menu = json["menu"];
  configFile.close();
}

void Config::setChanged() {
  changed = true;
}

void Config::storeToDisk() {
  if (changed) {
    std::ofstream configFile(HOME "config.json");
    nlohmann::json json;
    to_json(json, *this);
    configFile << std::setw(4) << json << std::endl;
    configFile.close();
  }
}
