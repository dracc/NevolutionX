#include "subsystems.h"

#ifdef NXDK
#include <nxdk/mount.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/video.h>
#include <hal/xbox.h>
#endif

#include <SDL.h>
#include <SDL_ttf.h>

#include "outputLine.h"

int init_systems() {
#ifdef NXDK
  VIDEO_MODE xmode;
  void *p = NULL;
  while (XVideoListModes(&xmode, 0, 0, &p)) {}
  XVideoSetMode(xmode.width, xmode.height, xmode.bpp, xmode.refresh);

  if (!nxMountDrive('C', "\\Device\\Harddisk0\\Partition2")) {
    outputLine("Mounting error: Could not mount drive C\n");
  }
  if (!nxMountDrive('E', "\\Device\\Harddisk0\\Partition1")) {
    outputLine("Mounting error: Could not mount drive E\n");
  }
  if (!nxMountDrive('F', "\\Device\\Harddisk0\\Partition6")) {
    outputLine("Mounting error: Could not mount drive F\n");
  }
  if (!nxMountDrive('G', "\\Device\\Harddisk0\\Partition7")) {
    outputLine("Mounting error: Could not mount drive G\n");
  }
  if (!nxMountDrive('X', "\\Device\\Harddisk0\\Partition3")) {
    outputLine("Mounting error: Could not mount drive X\n");
  }
  if (!nxMountDrive('Y', "\\Device\\Harddisk0\\Partition4")) {
    outputLine("Mounting error: Could not mount drive Y\n");
  }
  if (!nxMountDrive('Z', "\\Device\\Harddisk0\\Partition5")) {
    outputLine("Mounting error: Could not mount drive Z\n");
  }
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
  SDL_Delay(2000);
  XReboot();
#endif
}
