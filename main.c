#ifdef NXDK
#include <hal/xbox.h>
#endif

#include "outputLine.h"
#include "navigateMenu.h"
#include "subsystems.h"

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
