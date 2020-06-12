#include <NevolutionX/sys/subsystems.h>

#include <NevolutionX/debug/outputLine.h>
#include <NevolutionX/ftp/ftpServer.h>

#ifdef NXDK
#include <NevolutionX/net/networking.h>

#include <nxdk/mount.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/video.h>
#include <hal/xbox.h>
#endif

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef NXDK
#define PORT 21
#else
#define PORT 2121
#endif
#define USE_DHCP 1
#define _PCRTC_START        0xFD600800


extern "C" {
  extern uint8_t* _fb;
}

int init_input() {
  // Open our GameController
  SDL_GameController *sgc = SDL_GameControllerOpen(0);
  if (sgc == nullptr) {
    outputLine("Joystick Error: %s", SDL_GetError());
    SDL_Delay(2000);
  }

  // Set a hint that we want to use our gamecontroller always
  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
  return 0;
}

int init_drives() {
  int error = 0;
#ifdef NXDK
  if (!nxMountDrive('C', "\\Device\\Harddisk0\\Partition2")) {
    outputLine("Mounting error: Could not mount drive C\n");
    error = 1;
  }
  if (!nxMountDrive('E', "\\Device\\Harddisk0\\Partition1")) {
    outputLine("Mounting error: Could not mount drive E\n");
    error = 1;
  }
  if (!nxMountDrive('F', "\\Device\\Harddisk0\\Partition6")) {
    outputLine("Mounting error: Could not mount drive F\n");
    error = 1;
  }
  if (!nxMountDrive('G', "\\Device\\Harddisk0\\Partition7")) {
    outputLine("Mounting error: Could not mount drive G\n");
    error = 1;
  }
  if (!nxMountDrive('X', "\\Device\\Harddisk0\\Partition3")) {
    outputLine("Mounting error: Could not mount drive X\n");
    error = 1;
  }
  if (!nxMountDrive('Y', "\\Device\\Harddisk0\\Partition4")) {
    outputLine("Mounting error: Could not mount drive Y\n");
    error = 1;
  }
  if (!nxMountDrive('Z', "\\Device\\Harddisk0\\Partition5")) {
    outputLine("Mounting error: Could not mount drive Z\n");
    error = 1;
  }
#endif
  return error;
}

int init_video() {
#ifdef NXDK
  VIDEO_MODE xmode;
  void *p = NULL;
  while (XVideoListModes(&xmode, 0, 0, &p)) {}
  XVideoSetMode(xmode.width, xmode.height, xmode.bpp, xmode.refresh);

  size_t fb_size = xmode.width * xmode.height * (xmode.bpp + 7) / 8;
  _fb = (uint8_t*)MmAllocateContiguousMemoryEx(fb_size, 0, 0xFFFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);
  memset(_fb, 0x00, fb_size);
  *(unsigned int*)(_PCRTC_START) = (unsigned int)_fb & 0x03FFFFFF;
#endif
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER) != 0) {
    outputLine("Init error: %s", SDL_GetError());
    return 3;
  }

  if (TTF_Init() != 0) {
    outputLine("TTF Init Error: %s", TTF_GetError());
    return 2;
  }
  return 0;
}

int init_network(void *) {
  ftpServer *s = nullptr;
#ifdef NXDK
  if (setupNetwork(USE_DHCP) != 0) {
    outputLine("Network setup failed.");
    return 1;
  }
#endif
  s = new ftpServer(PORT);
  s->init();

  // Runs 'forever'
  s->run();
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
