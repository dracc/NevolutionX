#include "subsystems.h"

#include "outputLine.h"

#ifdef NXDK
#include <nxdk/mount.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/video.h>
#include <hal/xbox.h>
#endif

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef NXDK
#include "networking.h"
#endif

extern "C" {
  extern uint8_t* _fb;
}

int init_systems() {
#ifdef NXDK
  VIDEO_MODE xmode;
  void *p = NULL;
  bool use_dhcp = true;
  while (XVideoListModes(&xmode, 0, 0, &p)) {}
  XVideoSetMode(xmode.width, xmode.height, xmode.bpp, xmode.refresh);

  size_t fb_size = xmode.width * xmode.height * (xmode.bpp + 7) / 8;
  _fb = (uint8_t*)MmAllocateContiguousMemoryEx(fb_size, 0, 0xFFFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);
  memset(_fb, 0x00, fb_size);
#define _PCRTC_START				0xFD600800
  *(unsigned int*)(_PCRTC_START) = (unsigned int)_fb & 0x03FFFFFF;

  if (!nxMountDrive('C', "\\Device\\Harddisk0\\Partition2")) {
    outputLine("Mounting error: Could not mount drive C\n");
  }
  if (!nxMountDrive('E', "\\Device\\Harddisk0\\Partition1")) {
    outputLine("Mounting error: Could not mount drive E\n");
  }
  if (!nxMountDrive('F', "\\Device\\Harddisk0\\Partition6")) {
    outputLine("Mounting warning: Could not mount drive F\n");
  }
  if (!nxMountDrive('G', "\\Device\\Harddisk0\\Partition7")) {
    outputLine("Mounting warning: Could not mount drive G\n");
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
  if (!nxMountDrive('A', "\\Device\\CdRom0")) {
    outputLine("Mounting warning: Could not mount DVD drive\n");
  }
#endif
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER) != 0) {
    outputLine("Init error: %s", SDL_GetError());
    return 3;
  }

  if (TTF_Init() != 0) {
    outputLine("TTF Init Error: %s", TTF_GetError());
    return 2;
  }
#ifdef NXDK
  if (setupNetwork(&use_dhcp) != 0) {
    outputLine("Network setup failed.");
    return 1;
  }
#endif
  return 0;
}

void shutdown_systems(int systems) {
#ifdef NXDK
  if (systems <= 1) {
    closeNetwork();
  }
#endif
  if (systems <= 2) {
    TTF_Quit();
  }
  if (systems <= 3) {
    SDL_Quit();
  }
#ifdef NXDK
  XReboot();
#endif
}
