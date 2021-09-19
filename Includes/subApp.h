#ifndef NEVOLUTIONX_INCLUDES_SUBAPP_H_
#define NEVOLUTIONX_INCLUDES_SUBAPP_H_

#include "font.h"

class SubApp {
public:
  virtual ~SubApp() = default;

  virtual void render(Font &font) = 0;

  inline void setActivePlayerID(int val) { activePlayerID = val; }

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

protected:
  // The contextual index of the gamepad that spawned the event being handled.
  //
  // This value is only relevant in the scope of one of the event handlers and
  // is undefined otherwise.
  int activePlayerID{-1};
};

#endif //NEVOLUTIONX_INCLUDES_SUBAPP_H_
