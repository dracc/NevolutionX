#ifndef NEVOLUTIONX_INCLUDES_SUBAPPROUTER_H_
#define NEVOLUTIONX_INCLUDES_SUBAPPROUTER_H_

#include <SDL.h>
#include <memory>
#include <stack>
#include "font.hpp"
#include "subApp.hpp"

#ifdef NXDK
#include <windows.h>
#endif

// Provides simple stack-based input and display routing.
//
// Maintains a stack of SubApp instances and routes rendering requests and
// input events to the topmost SubApp.
class SubAppRouter {
public:
  static SubAppRouter* getInstance();

  void push(const std::shared_ptr<SubApp>&);
  void pop();

  void render(Font& font);

  void handleAxisMotion(const SDL_ControllerAxisEvent& event);
  void handleButtonDown(const SDL_ControllerButtonEvent& event);
  void handleButtonUp(const SDL_ControllerButtonEvent& event);

private:
  static SubAppRouter* singleton;
  SubAppRouter();
#ifdef NXDK
  void processButtonRepeatEvents();
#endif

  std::stack<std::shared_ptr<SubApp>> subAppStack;

  // Percentage of full deflection that an analog stick must be moved before being
  // considered an intentional input.
  float analogStickDeadzone{ 0.38f };

  // Percentage of full deflection that a trigger must be moved before being considered an
  // intentional input.
  float triggerDeadzone{ 0.05f };

#ifdef NXDK
  LONGLONG ticksPerMillisecond;
  LONGLONG lastFrameStartTicks;

  // Maps {(PlayerID, Button), timestamp} to track when virtual button press events should
  // be fired while a button is held down.
  std::map<std::pair<int, SDL_GameControllerButton>, LONGLONG> buttonRepeatTimers;

  // Maps {(PlayerID, Button), value} to track the value of analog axes in order to emit
  // virtual digital button events.
  std::map<std::pair<int, SDL_GameControllerAxis>, int> lastAxisState;
#endif
};

#endif // NEVOLUTIONX_INCLUDES_SUBAPPROUTER_H_
