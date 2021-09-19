#ifndef NEVOLUTIONX_INCLUDES_SUBAPP_H_
#define NEVOLUTIONX_INCLUDES_SUBAPP_H_

#include <map>
#include <SDL.h>

#include "font.h"

class SubApp {
public:
  virtual ~SubApp() = default;

  virtual void render(Font &font) = 0;

  inline void setActivePlayerID(int val) { activePlayerID = val; }
  inline void setRepeatEvent(bool val) { isRepeatEvent = val; }

  // Invoked when this handler becomes the active input handler.
  virtual void onFocus() {}
  // Invoked when this handler becomes inactive.
  virtual void onBlur() {}

  // DPAD up
  virtual void onUpPressed() {}
  virtual void onUpReleased() {}

  // DPAD down
  virtual void onDownPressed() {}
  virtual void onDownReleased() {}

  // DPAD left
  virtual void onLeftPressed() {}
  virtual void onLeftReleased() {}

  // DPAD right
  virtual void onRightPressed() {}
  virtual void onRightReleased() {}

  virtual void onLeftStickPressed() {}
  virtual void onLeftStickReleased() {}
  virtual void onRightStickPressed() {}
  virtual void onRightStickReleased() {}

  virtual void onBackPressed() {}
  virtual void onBackReleased() {}
  virtual void onStartPressed() {}
  virtual void onStartReleased() {}

  virtual void onAPressed() {}
  virtual void onAReleased() {}
  virtual void onBPressed() {}
  virtual void onBReleased() {}
  virtual void onXPressed() {}
  virtual void onXReleased() {}
  virtual void onYPressed() {}
  virtual void onYReleased() {}
  virtual void onWhitePressed() {}
  virtual void onWhiteReleased() {}
  virtual void onBlackPressed() {}
  virtual void onBlackReleased() {}

  int getAutoRepeatInterval(SDL_GameControllerButton button) const {
    auto it = autoRepeatIntervals.find(button);
    if (it == autoRepeatIntervals.end()) {
      return 0;
    }
    return it->second;
  }

protected:
  // The contextual index of the gamepad that spawned the event being handled.
  //
  // This value is only relevant in the scope of one of the event handlers and is undefined otherwise.
  int activePlayerID{-1};

  // Set to true if the event being handled was spawned due to a button being held down.
  //
  // This value is only relevant in the scope of one of the event handlers and is undefined otherwise.
  bool isRepeatEvent{false};

  // Map of {button ID, repeat interval milliseconds} identifying buttons that will automatically generate repeated
  // Pressed/Released messages while being held down.
  std::map<SDL_GameControllerButton, int> autoRepeatIntervals;
};

#endif //NEVOLUTIONX_INCLUDES_SUBAPP_H_
