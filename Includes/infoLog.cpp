#include "infoLog.h"

#include <cstdarg>
#include <cstdio>

#ifdef NXDK
#include <windows.h>
#include <hal/debug.h>
#else
#include <SDL.h>
#endif

InfoLog *InfoLog::singleton = nullptr;

InfoLog *InfoLog::getInstance() {
  if (!singleton) {
    singleton = new InfoLog();
  }

  return singleton;
}

void InfoLog::configure(const Config &config) {
  getInstance()->configure(config.settings.logging);
}

void InfoLog::configure(loggingConfig const& config) {
  overlayEnabled = config.getOverlayEnabled();
  framesPerOverlayItem = config.getOverlayDurationFrames();
  overlayAlpha = static_cast<uint8_t >(config.getOverlayBackgroundAlpha() * 0xFF);
}

void InfoLog::outputLine(std::string const& line) {
  OutputDebugStringA(line.c_str());

  if (isOutputCaptured()) {
    InfoLog::getInstance()->addLine(line);
  } else {
#ifdef NXDK
    debugPrint("%s", line.c_str());
#else
    printf("%s", line.c_str());
#endif
  }
}

void InfoLog::outputLine(const char* format, ...) {
  char buffer[4096];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);

  outputLine(std::string(buffer));

  va_end(args);
}

void InfoLog::addLine(const std::string &line) {
  std::lock_guard<std::mutex> lock(logMutex);
  log.push_back(line);

  overlayLog.emplace_back(line, framesPerOverlayItem);
}

void InfoLog::renderAsOverlay(Renderer &r, Font &font) {
  if (!overlayEnabled) {
    return;
  }

  // TODO: Consider centralizing definition of the renderer safe area.
  auto displayWidth = static_cast<float>(r.getWidth()) * 0.8f;
  auto startWidth = static_cast<float>(r.getWidth()) * 0.1f;
  auto displayHeight = static_cast<float>(r.getHeight()) * 0.9f;
  auto startHeight = static_cast<float>(r.getHeight()) * 0.1f;

  float bottom = displayHeight + startHeight;

  std::list<std::pair<std::string, float>> reversedOutput;
  for (auto it = begin(overlayLog), itEnd = end(overlayLog); it != itEnd; ) {
    auto &item = *it;

    const std::string &line = item.first;
    auto lineHeight = font.getColumnHeight(line, displayWidth);
    reversedOutput.push_front(std::make_pair(line, lineHeight));

    if (!--item.second) {
      it = overlayLog.erase(it);
    } else {
      ++it;
    }
  }


  r.setDrawColor(overlayRed, overlayGreen, overlayBlue, overlayAlpha);
  for (auto const& item : reversedOutput) {
    auto itemHeight = item.second;
    bottom -= itemHeight;
    if (bottom < startHeight) {
      break;
    }

    SDL_FRect backgroundArea{startWidth, bottom, displayWidth, itemHeight};
    r.fillRectangle(backgroundArea);
    font.drawColumn(item.first, std::make_pair(startWidth, bottom), displayWidth);
  }
}
