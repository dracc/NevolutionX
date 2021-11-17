#ifndef NEVOLUTIONX_INCLUDES_INFOLOG_H_
#define NEVOLUTIONX_INCLUDES_INFOLOG_H_

#include <atomic>
#include <list>
#include <mutex>
#include <string>
#include <utility>
#include "config.hpp"
#include "font.hpp"
#include "renderer.hpp"

class InfoLog {
public:
  enum Level
  {
    ERROR,
    WARNING,
    INFO,
    DEBUG
  };

  static void configure(Config const& config);
  static void capture() { getInstance()->captured = true; }

  static void outputLine(Level level, const char* format, ...);
  static void outputLine(Level level, std::string const& line);

  static bool isOutputCaptured() { return getInstance()->captured; }

  static void renderOverlay(Renderer& r, Font& font) {
    getInstance()->renderAsOverlay(r, font);
  }

  static int getLogSize();
  static std::mutex& getLogMutex();
  static const std::list<std::string>& getLog();

  static Level getOverlayLogLevel();
  static void setOverlayLogLevel(Level level);

private:
  // Tuple of string, frames remaining.
  typedef std::pair<std::string, int> OverlayItem;

  static InfoLog* getInstance();

  InfoLog() = default;
  void configure(loggingConfig const& config);
  void addLine(Level level, std::string const& line);
  void renderAsOverlay(Renderer& r, Font& font);

  static InfoLog* singleton;

  // The number of frames to display an overlay log entry before hiding it.
  int framesPerOverlayItem{ 30 };

  bool overlayEnabled{ true };
  uint8_t overlayRed{ 0 };
  uint8_t overlayGreen{ 0 };
  uint8_t overlayBlue{ 0 };
  uint8_t overlayAlpha{ 140 };

  std::atomic<bool> captured{ false };
  std::mutex logMutex;
  std::list<std::string> log;
  std::list<OverlayItem> overlayLog;

  Level overlayLogLevel{ INFO };
};

#endif // NEVOLUTIONX_INCLUDES_INFOLOG_H_
