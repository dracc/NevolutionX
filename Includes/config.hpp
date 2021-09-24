#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
#include "../3rdparty/json.hpp"
#pragma clang diagnostic pop


#ifdef NXDK
class netConfig {
  bool enable;
  bool useDHCP;
  unsigned int staticGateway;
  unsigned int staticIP;
  unsigned int staticNetmask;

public:
  netConfig();

  bool getEnabled() const { return enable; }
  bool getUseDHCP() const { return useDHCP; }
  unsigned int getStaticGateway() const { return staticGateway; }
  unsigned int getStaticIP() const { return staticIP; }
  unsigned int getStaticNetmask() const { return staticNetmask; }

  void setEnabled(bool val) { enable = val; }
  void setUseDHCP(bool val) { useDHCP = val; }
  void setStaticGateway(unsigned int val) { staticGateway = val; }
  void setStaticIP(unsigned int val) { staticIP = val; }
  void setStaticNetmask(unsigned int val) { staticNetmask = val; }
};
void to_json(nlohmann::json& j, netConfig const& o);
void from_json(nlohmann::json const& j, netConfig& o);

class sntpConfig {
  bool enable{ false };
  std::string server{ "pool.ntp.org" };
  unsigned int port{ 123 };

public:
  sntpConfig() = default;

  bool getEnabled() const { return enable; }
  std::string const& getServer() const { return server; }
  unsigned int getPort() const { return port; }

  void setEnabled(bool val) { enable = val; }
  void setServer(std::string const& val) { server = val; }
  void setPort(unsigned int val) { port = val; }
};
void to_json(nlohmann::json& j, sntpConfig const& o);
void from_json(nlohmann::json const& j, sntpConfig& o);
#endif

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

class loggingConfig {
  std::string overlayLogLevel{ "WARNING" };
  bool enableOverlay{ true };
  int overlayDurationFrames{ 30 };
  float overlayBackgroundAlpha{ 0.75f };

public:
  loggingConfig() = default;

  std::string getOverlayLogLevel() const { return overlayLogLevel; }
  bool getOverlayEnabled() const { return enableOverlay; }
  int getOverlayDurationFrames() const { return overlayDurationFrames; }
  float getOverlayBackgroundAlpha() const { return overlayBackgroundAlpha; }

  void setOverlayLogLevel(const std::string& level) { overlayLogLevel = level; }
  void setOverlayEnabled(bool val) { enableOverlay = val; }
  void setOverlayDurationFrames(int val) { overlayDurationFrames = val; }
  void setOverlayBackgroundAlpha(float val) { overlayBackgroundAlpha = val; }
};
void to_json(nlohmann::json& j, loggingConfig const& o);
void from_json(nlohmann::json const& j, loggingConfig& o);

class homescreenConfig {
  bool showFPS{ false };
  bool showIP{ true };

public:
  homescreenConfig() = default;

  bool getShowFPS() const { return showFPS; }
  bool getShowIP() const { return showIP; }

  void setShowFPS(bool val) { showFPS = val; }
  void setShowIP(bool val) { showIP = val; }
};
void to_json(nlohmann::json& j, homescreenConfig const& o);
void from_json(nlohmann::json const& j, homescreenConfig& o);

class Settings {
public:
  std::string activeThemeDirectory{ "default" };
#ifdef NXDK
  netConfig net;
  sntpConfig sntp;
#endif
  ftpConfig ftp;
  mountConfig mount;
  loggingConfig logging;
  homescreenConfig homescreen;
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
