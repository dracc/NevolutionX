#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../3rdparty/json.hpp"
#include <string>

class ftpConfig {
  bool enable;
  std::string username;
  std::string password;
  int port;
public:
  ftpConfig();

  bool getEnabled() const { return enable; }
  const std::string& getUser() const { return username; }
  const std::string& getPassword() const { return password; }
  int getPort() const { return port; }

  void setEnabled(bool val);
  void setUser(std::string const& username);
  void setPassword(std::string const& password);
  void setPort(int port);
};
void to_json(nlohmann::json& j, ftpConfig const& f);
void from_json(nlohmann::json const& j, ftpConfig& f);

class mountConfig {
  bool enableF;
  bool enableG;

 public:
  mountConfig();

  bool getFEnabled() const { return enableF; }
  bool getGEnabled() const { return enableG; }

  void setFEnabled(bool val) { enableF = val; }
  void setGEnabled(bool val) { enableG = val; }
};
void to_json(nlohmann::json& j, mountConfig const& o);
void from_json(nlohmann::json const& j, mountConfig& o);

class Settings {
public:
  Settings() = default;
  ftpConfig ftp;
  mountConfig mount;
};
void to_json(nlohmann::json& j, Settings const& o);
void from_json(nlohmann::json const& j, Settings& o);

class Config {
  bool changed = false;
public:
  Config();

  void setChanged();
  void storeToDisk();
  Settings settings;
  nlohmann::json menu;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, menu, settings)

#endif
