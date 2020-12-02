#include <vector>
#include "config.hpp"
#include "menu.hpp"
#include "langMenu.hpp"
#include "timeMenu.hpp"
#include "findXBE.h"
#include "font.h"
#include "outputLine.h"
#include "renderer.h"
#include "subsystems.h"

#include "ftpServer.h"

#include <memory>
#include <type_traits>
#include <thread>
#include <SDL.h>

#ifdef NXDK
#include <hal/xbox.h>
#include <hal/video.h>
#include <windows.h>
#endif

#ifdef NXDK
#define SEPARATOR "\\"
#define HOME "A:" SEPARATOR
#else
#define SEPARATOR "/"
#define HOME "." SEPARATOR
#endif

int main(void) {
#ifdef NXDK
  mountHomeDir('A');
#endif
  Config config;
  std::map<int, SDL_GameController*> controllers;

  int init = init_systems();
  ftpServer *s = nullptr;

  std::thread thrF;

  if (init <= 1) {
    bool running = true;

    // Open our GameController
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
      if (SDL_IsGameController(i)) {
        controllers[i] = SDL_GameControllerOpen(i);
        if (!controllers[i]) {
          outputLine("Could not open gamecontroller %i: %s\n", i, SDL_GetError());
          SDL_Delay(2000);
        }
      }
    }

    // Set a hint that we want to use our gamecontroller always
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (init == 0 && config.settings.ftp.getEnabled()) {
      s = new ftpServer(&config.settings.ftp);
      s->init();
      thrF = std::thread(thread_runner, s);
    }

    // Create render system
    Renderer r;
    r.init(HOME);

    // Load font
    // FIXME: Font path should be read from theme
    Font f(r, HOME "vegur.ttf");

    Menu menu(config, r);
    std::shared_ptr<MenuNode> lang = nullptr;
    std::shared_ptr<MenuNode> timeZone = nullptr;

    r.drawBackground();
    r.flip();

    SDL_Event event;

#ifdef NXDK
    ULONG ValueIndex = 0;
    ULONG Type = 0;
    uint32_t Value = 0;
    char Value2[5] = {0};
    ULONG ValueLength = 4;
    ULONG ResultLength = 0;
    ValueIndex = 0x1;
    ExQueryNonVolatileSetting(ValueIndex, &Type, &Value2,
                              ValueLength, &ResultLength);
    if (ValueLength == ResultLength && Value2[0] == 0) {
      timeZone = std::make_shared<TimeMenu>(menu.getCurrentMenu(), "Timezone select");
      menu.setCurrentMenu(timeZone.get());
    }
    ValueIndex = 0x7;
    ExQueryNonVolatileSetting(ValueIndex, &Type, &Value,
                              ValueLength, &ResultLength);
    if (ValueLength == ResultLength && Value == 0) {
      lang = std::make_shared<LangMenu>(menu.getCurrentMenu(), "Language select");
      menu.setCurrentMenu(lang.get());
    }
#endif

    while (running) {
      r.setDrawColor(0, 89, 0);
      r.clear();
      r.drawBackground();

      menu.render(f);
      r.flip();
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
          break;
        } else if (event.type == SDL_CONTROLLERDEVICEADDED) {
          controllers[event.cdevice.which] = SDL_GameControllerOpen(event.cdevice.which);
        } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
          SDL_GameControllerClose(controllers[event.cdevice.which]);
          controllers.erase(event.cdevice.which);
        } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
          if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
            menu.up();
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
            menu.down();
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
            menu.execute();
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B ||
                     event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) {
            menu.back();
          }
        }
      }
#ifdef NXDK
      // Let's not hog CPU for nothing.
      SwitchToThread();
#endif
    }
  }
  for (auto c: controllers) {
    SDL_GameControllerClose(c.second);
  }
  delete s;
  shutdown_systems(init);
  return init;
}
