#ifndef NEVOLUTIONX_INCLUDES_INFOLOG_H_
#define NEVOLUTIONX_INCLUDES_INFOLOG_H_

#include <list>
#include <mutex>
#include <string>
#include <utility>

#include "config.hpp"
#include "font.h"
#include "renderer.h"

class InfoLog {
public:
  static void configure(Config const& config);
  static void capture() { getInstance()->captured = true; }

  static void outputLine(const char* format, ...);
  static void outputLine(std::string const& line);

  static bool isOutputCaptured() { return getInstance()->captured; }

  static void renderOverlay(Renderer &r, Font &font) {
    getInstance()->renderAsOverlay(r, font);
  }

private:
  // Tuple of string, frames remaining.
  typedef std::pair<std::string, int> OverlayItem;

  static InfoLog *getInstance();

  InfoLog() = default;
  void configure(loggingConfig const& config);
  void addLine(std::string const& line);
  void renderAsOverlay(Renderer &r, Font &font);

  static InfoLog *singleton;

  // The number of frames to display an overlay log entry before hiding it.
  int framesPerOverlayItem{30};

  bool overlayEnabled{true};
  uint8_t overlayRed{0};
  uint8_t overlayGreen{0};
  uint8_t overlayBlue{0};
  uint8_t overlayAlpha{140};
  
  std::atomic<bool> captured{false};
  std::mutex logMutex;
  std::list<std::string> log;
  std::list<OverlayItem> overlayLog;
};

#endif //NEVOLUTIONX_INCLUDES_INFOLOG_H_
