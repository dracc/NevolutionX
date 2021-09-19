#include "subAppRouter.h"

#include "outputLine.h"

SubAppRouter *SubAppRouter::singleton = nullptr;

SubAppRouter *SubAppRouter::getInstance() {
  if (!singleton) {
    singleton = new SubAppRouter();
  }
  return singleton;
}

static int getPlayerIndex(SDL_JoystickID id) {
  return SDL_JoystickGetPlayerIndex(SDL_JoystickFromInstanceID(id));
}

void SubAppRouter::push(const std::shared_ptr<SubApp>& app) {
  if (!subAppStack.empty()) {
    subAppStack.top()->onBlur();
  }
  subAppStack.push(app);
  app->onFocus();
}

void SubAppRouter::pop() {
  if (subAppStack.size() == 1) {
    outputLine("Attempt to pop only SubApp, ignoring...");
    return;
  }

  subAppStack.top()->onBlur();
  subAppStack.pop();
  subAppStack.top()->onFocus();
}

void SubAppRouter::render(Font &font) {
  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp &app = *ref;
  app.render(font);
}

void SubAppRouter::handleAxisMotion(const SDL_ControllerAxisEvent &event) {
  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp &app = *ref;
  app.setActivePlayerID(getPlayerIndex(event.which));
  // TODO: Translate axis events.
}

void SubAppRouter::handleButtonDown(const SDL_ControllerButtonEvent &event) {
  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp &app = *ref;
  app.setActivePlayerID(getPlayerIndex(event.which));

  switch (event.button) {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      app.onUpPressed();
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      app.onDownPressed();
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
      app.onLeftPressed();
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
      app.onRightPressed();
      break;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
      app.onLeftStickPressed();
      break;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
      app.onRightStickPressed();
      break;
    case SDL_CONTROLLER_BUTTON_A:
      app.onAPressed();
      break;
    case SDL_CONTROLLER_BUTTON_B:
      app.onBPressed();
      break;
    case SDL_CONTROLLER_BUTTON_X:
      app.onXPressed();
      break;
    case SDL_CONTROLLER_BUTTON_Y:
      app.onYPressed();
      break;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
      app.onWhitePressed();
      break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
      app.onBlackPressed();
      break;
    case SDL_CONTROLLER_BUTTON_BACK:
      app.onBackPressed();
      break;
    case SDL_CONTROLLER_BUTTON_START:
      app.onStartPressed();
      break;
  }
}

void SubAppRouter::handleButtonUp(const SDL_ControllerButtonEvent &event) {
  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp &app = *ref;
  app.setActivePlayerID(getPlayerIndex(event.which));

  switch (event.button) {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      app.onUpReleased();
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      app.onDownReleased();
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
      app.onLeftReleased();
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
      app.onRightReleased();
      break;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
      app.onLeftStickReleased();
      break;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
      app.onRightStickReleased();
      break;
    case SDL_CONTROLLER_BUTTON_A:
      app.onAReleased();
      break;
    case SDL_CONTROLLER_BUTTON_B:
      app.onBReleased();
      break;
    case SDL_CONTROLLER_BUTTON_X:
      app.onXReleased();
      break;
    case SDL_CONTROLLER_BUTTON_Y:
      app.onYReleased();
      break;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
      app.onWhiteReleased();
      break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
      app.onBlackReleased();
      break;
    case SDL_CONTROLLER_BUTTON_BACK:
      app.onBackReleased();
      break;
    case SDL_CONTROLLER_BUTTON_START:
      app.onStartReleased();
      break;
  }
}
