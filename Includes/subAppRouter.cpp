#include "subAppRouter.h"
#include <list>
#include "infoLog.h"

#define ANALOG_MAX_VALUE 32767.0f

static void routeButtonDown(SubApp& app, int playerID, SDL_GameControllerButton button);
static void routeButtonUp(SubApp& app, int playerID, SDL_GameControllerButton button);

SubAppRouter* SubAppRouter::singleton = nullptr;

SubAppRouter* SubAppRouter::getInstance() {
  if (!singleton) {
    singleton = new SubAppRouter();
  }
  return singleton;
}

SubAppRouter::SubAppRouter() {
  LARGE_INTEGER frequency;
  if (!QueryPerformanceFrequency(&frequency)) {
    InfoLog::outputLine(
        "Failed to query performance frequency, timing dependent operations will fail. "
        "%d\n",
        GetLastError());
    frequency.QuadPart = 0;
  }
  ticksPerMillisecond = frequency.QuadPart / 1000;

  LARGE_INTEGER counter;
  if (!QueryPerformanceCounter(&counter)) {
    InfoLog::outputLine("Failed to query performance counter %d\n", GetLastError());
    counter.QuadPart = 0;
  }
  lastFrameStartTicks = counter.QuadPart;
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
    InfoLog::outputLine("Attempt to pop only SubApp, ignoring...");
    return;
  }

  subAppStack.top()->onBlur();
  subAppStack.pop();
  subAppStack.top()->onFocus();
}

void SubAppRouter::render(Font& font) {
  if (ticksPerMillisecond) {
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
      counter.QuadPart = 0;
    }
    LONGLONG ticksElapsed = counter.QuadPart - lastFrameStartTicks;
    lastFrameStartTicks = counter.QuadPart;
  }

  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp& app = *ref;
  app.render(font);

  processButtonRepeatEvents();
}

void SubAppRouter::handleAxisMotion(const SDL_ControllerAxisEvent& event) {
  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp& app = *ref;
  int playerID = getPlayerIndex(event.which);
  app.setActivePlayerID(playerID);

  int value = event.value;
  if (event.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT
      || event.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
    if (value < static_cast<int>(triggerDeadzone * ANALOG_MAX_VALUE)) {
      value = 0;
    }
  } else {
    if (abs(value) < static_cast<int>(analogStickDeadzone * ANALOG_MAX_VALUE)) {
      value = 0;
    } else if (value < static_cast<int>(-ANALOG_MAX_VALUE)) {
      value = -ANALOG_MAX_VALUE;
    }
  }

  auto lastStateKey = std::make_pair(playerID,
                                     static_cast<SDL_GameControllerAxis>(event.axis));
  auto lastStateIterator = lastAxisState.find(lastStateKey);

  if (lastStateIterator == lastAxisState.end()) {
    // Insert a dummy value that will be treated as a change in state if this is not a
    // deadzone event.
    lastStateIterator = lastAxisState.insert(std::make_pair(lastStateKey, 0)).first;
  }

  // Ignore duplicated events.
  if (lastStateIterator->second == value) {
    return;
  }

  switch (event.axis) {
  case SDL_CONTROLLER_AXIS_LEFTX:
    app.onLeftStickXChanged(value);

    if (lastStateIterator->second > 0) {
      app.onLeftStickDigitalRightReleased();
    } else if (lastStateIterator->second < 0) {
      app.onLeftStickDigitalLeftReleased();
    }

    if (value > 0) {
      app.onLeftStickDigitalRightPressed();
    } else if (value < 0) {
      app.onLeftStickDigitalLeftPressed();
    }

    break;

  case SDL_CONTROLLER_AXIS_LEFTY:
    app.onLeftStickYChanged(value);

    if (lastStateIterator->second > 0) {
      app.onLeftStickDigitalDownReleased();
    } else if (lastStateIterator->second < 0) {
      app.onLeftStickDigitalUpReleased();
    }

    if (value > 0) {
      app.onLeftStickDigitalDownPressed();
    } else if (value < 0) {
      app.onLeftStickDigitalUpPressed();
    }

    break;

  case SDL_CONTROLLER_AXIS_RIGHTX:
    app.onRightStickXChanged(value);

    if (lastStateIterator->second > 0) {
      app.onRightStickDigitalRightReleased();
    } else if (lastStateIterator->second < 0) {
      app.onRightStickDigitalLeftReleased();
    }

    if (value > 0) {
      app.onRightStickDigitalRightPressed();
    } else if (value < 0) {
      app.onRightStickDigitalLeftPressed();
    }

    break;

  case SDL_CONTROLLER_AXIS_RIGHTY:
    app.onRightStickYChanged(value);

    if (lastStateIterator->second > 0) {
      app.onRightStickDigitalDownReleased();
    } else if (lastStateIterator->second < 0) {
      app.onRightStickDigitalUpReleased();
    }

    if (value > 0) {
      app.onRightStickDigitalDownPressed();
    } else if (value < 0) {
      app.onRightStickDigitalUpPressed();
    }

    break;

  case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
    app.onLeftTriggerChanged(value);
    break;

  case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
    app.onRightTriggerChanged(value);
    break;
  }

  lastAxisState[lastStateKey] = value;
}

void SubAppRouter::handleButtonDown(const SDL_ControllerButtonEvent& event) {
  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp& app = *ref;
  int playerID = getPlayerIndex(event.which);

  auto button = static_cast<SDL_GameControllerButton>(event.button);

  int autoRepeatInterval = app.getAutoRepeatInterval(button);
  if (autoRepeatInterval > 0) {
    buttonRepeatTimers[std::make_pair(playerID, button)] =
        lastFrameStartTicks + ticksPerMillisecond * autoRepeatInterval;
  }

  app.setRepeatEvent(false);
  routeButtonDown(app, playerID, button);
}

void SubAppRouter::handleButtonUp(const SDL_ControllerButtonEvent& event) {
  assert(!subAppStack.empty());
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp& app = *ref;
  int playerID = getPlayerIndex(event.which);
  auto button = static_cast<SDL_GameControllerButton>(event.button);

  buttonRepeatTimers.erase(std::make_pair(playerID, button));

  app.setRepeatEvent(false);
  routeButtonUp(app, playerID, button);
}

void SubAppRouter::processButtonRepeatEvents() {
  std::shared_ptr<SubApp> ref = subAppStack.top();
  SubApp& app = *ref;

  std::list<std::pair<int, SDL_GameControllerButton>> deleteList;

  for (auto& it: buttonRepeatTimers) {
    auto& timestamp = it.second;
    if (timestamp > lastFrameStartTicks) {
      continue;
    }

    const auto& playerID = it.first.first;
    const auto& button = it.first.second;

    app.setRepeatEvent(true);
    routeButtonUp(app, playerID, button);
    routeButtonDown(app, playerID, button);

    LONGLONG repeatInterval = app.getAutoRepeatInterval(button);
    if (!repeatInterval) {
      deleteList.push_back(it.first);
    } else {
      timestamp = lastFrameStartTicks + repeatInterval * ticksPerMillisecond;
    }
  }

  for (auto& deleteKey: deleteList) {
    buttonRepeatTimers.erase(deleteKey);
  }
}

static void routeButtonDown(SubApp& app, int playerID, SDL_GameControllerButton button) {
  app.setActivePlayerID(playerID);
  switch (button) {
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
  default:
    InfoLog::outputLine("Ignoring invalid button ID %d\n", button);
    break;
  }
}

static void routeButtonUp(SubApp& app, int playerID, SDL_GameControllerButton button) {
  app.setActivePlayerID(playerID);
  switch (button) {
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
  default:
    InfoLog::outputLine("Ignoring invalid button ID %d\n", button);
    break;
  }
}
