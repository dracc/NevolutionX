#include "screensaver.hpp"
#include "timing.hpp"

Screensaver::Screensaver(Renderer& r, bool enabled, unsigned int timeoutMillis) :
    renderer(r), timeoutMillis(timeoutMillis), isEnabled(enabled) {
  lastInputTime = std::chrono::steady_clock::now();
}

void Screensaver::render() {
  if (!isEnabled) {
    return;
  }
  auto millisElapsed = millisSince(lastInputTime);
  if (millisElapsed >= timeoutMillis) {
    isActive = true;
  }

  if (!isActive) {
    return;
  }
  renderer.setDrawColor(0, 0, 0, 200);

  SDL_FRect screen = { 0, 0, static_cast<float>(renderer.getWidth()),
                       static_cast<float>(renderer.getHeight()) };
  renderer.fillRectangle(screen);
}

void Screensaver::notifyInput() {
  lastInputTime = std::chrono::steady_clock::now();
  isActive = false;
}
