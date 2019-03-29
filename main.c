#ifdef NXDK
#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#endif

#include "outputLine.h"
#include "navigateMenu.h"
#include "subsystems.h"

#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stdbool.h"
#include <SDL.h>
#include "SDL_ttf.h"
#include <threads.h>

#ifdef NXDK
int _exit(int x)
{
    outputLine("_exit called\n");
    outputLine( " SDL_Error: %s\n", SDL_GetError() );
    XSleep(5000);
    XReboot();
    return 0;
}
#endif

int main(void) {
  int init = init_systems();
  if (init == 0) {
    thrd_t thr;
    
    thrd_create(&thr, menuLoop, NULL);
    thrd_join(thr, NULL);
  }
  shutdown_systems(init);
  return init;
}
