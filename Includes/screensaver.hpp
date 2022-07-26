#ifndef NEVOLUTIONX_SCREENSAVER_H
#define NEVOLUTIONX_SCREENSAVER_H

#include <chrono>
#include "renderer.hpp"

class Screensaver {
public:
  Screensaver(Renderer& r, bool enable, unsigned int timeoutMillis);

  void render();

  // Checks to see if the screensaver should consume an input event and stop rendering.
  void notifyInput();

private:
  Renderer& renderer;
  unsigned int timeoutMillis;
  bool isEnabled{ true };
  bool isActive{ false };
  std::chrono::steady_clock::time_point lastInputTime;
};

#endif // NEVOLUTIONX_SCREENSAVER_H
