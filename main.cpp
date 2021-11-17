#include <SDL.h>
#include <memory>
#include <vector>
#include "config.hpp"
#include "font.hpp"
#include "ftpServer.hpp"
#include "infoLog.hpp"
#include "langMenu.hpp"
#include "menu.hpp"
#include "networkManager.hpp"
#include "renderer.hpp"
#include "sntpClient.hpp"
#include "subAppRouter.hpp"
#include "subsystems.hpp"
#include "timeMenu.hpp"
#include "timing.hpp"

#ifdef NXDK
#include <hal/video.h>
#include <windows.h>
#include "eeprom.hpp"
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

  InfoLog::configure(config);

  int init = init_systems(config);
  if (init) {
    shutdown_systems(init);
    return init;
  }

  NetworkManager networkManager(config);
#ifdef NXDK
  if (config.settings.net.getEnabled()) {
    networkManager.asyncInit();
  }
#else
  networkManager.asyncInit();
#endif

  bool running = true;

  // Open our GameController
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
      controllers[i] = SDL_GameControllerOpen(i);
      if (!controllers[i]) {
        InfoLog::outputLine(InfoLog::ERROR, "Could not open gamecontroller %i: %s\n", i,
                            SDL_GetError());
        SDL_Delay(2000);
      }
    }
  }

  // Set a hint that we want to use our gamecontroller always
  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

  // Create render system
  Renderer r;
  r.init(HOME);

  // Load font
  // FIXME: Font path should be read from theme
  Font f(r, HOME "vegur.ttf");

  SubAppRouter& router = *SubAppRouter::getInstance();

  auto menu = std::make_shared<Menu>(config, r);
  router.push(menu);

  std::shared_ptr<MenuNode> lang = nullptr;
  std::shared_ptr<MenuNode> timeZone = nullptr;

  r.drawBackground();
  r.flip();

  InfoLog::capture();

  SDL_Event event;

#ifdef NXDK
  ULONG ValueIndex = 0x1;
  uint32_t Value = getEEPROMValue<uint32_t>(ValueIndex);
  if (Value == 0) {
#endif
    timeZone = std::make_shared<TimeMenu>(menu->getCurrentMenu(), "Timezone select");
    menu->setCurrentMenu(timeZone.get());
#ifdef NXDK
  }
  ValueIndex = 0x7;
  Value = getEEPROMValue<uint32_t>(ValueIndex);
  if (Value == 0) {
#endif
    lang = std::make_shared<LangMenu>(menu->getCurrentMenu(), "Language select");
    menu->setCurrentMenu(lang.get());
#ifdef NXDK
  }
#endif

  int info_x = static_cast<int>(r.getWidth() * 0.70);
  int info_y = static_cast<int>(r.getHeight() * 0.85);

  ftpServer* ftpServerInstance = nullptr;
  std::shared_ptr<sntpClient> sntpClientInstance;

  auto lastFrameStart = std::chrono::steady_clock::now();
  while (running) {
    auto frameStart = std::chrono::steady_clock::now();
    if (networkManager.isNewlyInitialized()) {
      if (config.settings.ftp.getEnabled()) {
        ftpServerInstance = new ftpServer(&config.settings.ftp);
        ftpServerInstance->init();
        ftpServerInstance->runAsync();
      }

#ifdef NXDK
      if (config.settings.sntp.getEnabled()) {
        sntpClientInstance = std::make_shared<sntpClient>(config.settings.sntp.getServer(),
                                                          config.settings.sntp.getPort());
        sntpClientInstance->updateTimeAsync();
      }
#endif
    }

    r.setDrawColor(0, 89, 0);
    r.clear();
    r.drawBackground();

    router.render(f);

    std::pair<float, float> info_coordinates(info_x, info_y);
    if (config.settings.homescreen.getShowIP()) {
      std::string ip_address = networkManager.IPAddressString();
      auto draw_rect = f.draw(ip_address, info_coordinates);
      info_coordinates.second += draw_rect.second;
    }
    if (config.settings.homescreen.getShowFPS()) {
      int lastFrameDuration = static_cast<int>(millisBetween(lastFrameStart, frameStart));
      if (lastFrameDuration > 0) {
        int fps = 1000 / lastFrameDuration;
        char fpsBuffer[64] = { 0 };
        snprintf(fpsBuffer, 63, "FPS: %d (%dms)", fps, lastFrameDuration);
        f.draw(fpsBuffer, info_coordinates);
      }
    }

    InfoLog::renderOverlay(r, f);

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
        router.handleButtonDown(event.cbutton);
      } else if (event.type == SDL_CONTROLLERBUTTONUP) {
        router.handleButtonUp(event.cbutton);
      } else if (event.type == SDL_CONTROLLERAXISMOTION) {
        router.handleAxisMotion(event.caxis);
      }
    }

#ifdef NXDK
    // Let's not hog CPU for nothing.
    SwitchToThread();
#endif
    lastFrameStart = frameStart;
  }

  for (auto c: controllers) {
    SDL_GameControllerClose(c.second);
  }

  delete ftpServerInstance;

  shutdown_systems(init);
  return init;
}
