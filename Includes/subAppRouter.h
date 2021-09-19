#ifndef NEVOLUTIONX_INCLUDES_SUBAPPROUTER_H_
#define NEVOLUTIONX_INCLUDES_SUBAPPROUTER_H_

#include <stack>

#include <SDL.h>

#include "font.h"
#include "subApp.h"

// Provides simple stack-based input and display routing.
//
// Maintains a stack of SubApp instances and routes rendering requests and
// input events to the topmost SubApp.
class SubAppRouter {
public:
  static SubAppRouter *getInstance();

  void push(const std::shared_ptr<SubApp>&);
  void pop();

  void render(Font &font);

  void handleAxisMotion(const SDL_ControllerAxisEvent &event);
  void handleButtonDown(const SDL_ControllerButtonEvent &event);
  void handleButtonUp(const SDL_ControllerButtonEvent &event);

private:
  static SubAppRouter *singleton;

  std::stack<std::shared_ptr<SubApp>> subAppStack;
};

#endif //NEVOLUTIONX_INCLUDES_SUBAPPROUTER_H_
