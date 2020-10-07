#include <vector>
#include "config.hpp"
#include "menu.hpp"
#include "findXBE.h"
#include "font.h"
#include "xbeMenuItem.h"
#include "menuItem.h"
#include "outputLine.h"
#include "renderer.h"
#include "subsystems.h"

#include "ftpServer.h"

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

  int init = init_systems();
  ftpServer *s = nullptr;

  std::thread thrF;

  if (init <= 1) {
    bool running = true;

    // Open our GameController
    SDL_GameController *sgc = SDL_GameControllerOpen(0);
    if (sgc == nullptr) {
      outputLine("Joystick Error: %s", SDL_GetError());
      SDL_Delay(2000);
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

    r.drawBackground();
    r.flip();

    SDL_Event event;

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
  delete s;
  shutdown_systems(init);
  return init;
}
