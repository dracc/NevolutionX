#include "subsystems.h"

#ifdef NXDK
#include <hal/video.h>
#include <winapi/synchapi.h>
#endif

int init_systems() {
#ifdef NXDK
  VIDEO_MODE xmode;
  void *p = NULL;
  while (XVideoListModes(&xmode, 0, 0, &p)) {}
  XVideoSetMode(xmode.width, xmode.height, xmode.bpp, xmode.refresh);
#endif
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER) != 0) {
    outputLine("Init error: %s", SDL_GetError());
    return 2;
  }

  if (TTF_Init() != 0) {
    outputLine("TTF Init Error: %s", TTF_GetError());
    return 1;
  }
  return 0;
}

void shutdown_systems(int systems) {
  if (systems == 0) {
    TTF_Quit();
  }
  if (systems <= 1) {
    SDL_Quit();
  }
#ifdef NXDK
  Sleep(2000);
  XReboot();
#endif
}
