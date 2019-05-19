#include "subsystems.h"

#ifdef NXDK
#include <hal/video.h>
#endif

int init_systems() {
#ifdef NXDK
  XVideoSetMode(640,480,32,REFRESH_DEFAULT);
  XInput_Init();
  if (pb_init() != 0) {
    return 3;
  }
  pb_show_debug_screen();
#endif
  if (SDL_VideoInit(NULL) != 0) {
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
  XInput_Quit();
  if (systems <= 2) {
    pb_kill();
  }
  XSleep(2000);
  XReboot();
#endif
}
