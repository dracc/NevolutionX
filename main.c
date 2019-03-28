#ifdef NXDK
#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#endif

#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include <SDL.h>
#include <threads.h>

typedef struct menuItem{
  SDL_Texture* tex;
  bool selected;
} menuItem;

int main(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
#ifdef NXDK
    XSleep(2000);
    XReboot();
#else
    SDL_Log("Init error: %s", SDL_GetError());
#endif
    return 1;
  }

#ifdef NXDK
  XReboot();
#endif
  SDL_Quit();
  return 0;
}


#ifdef NXDK
#else
#endif
