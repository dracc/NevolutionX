#include "subsystems.hpp"
#include "infoLog.hpp"

#ifdef NXDK
#include <hal/video.h>
#include <hal/xbox.h>
#include <nxdk/mount.h>
#include <nxdk/path.h>
#include <pbkit/nv_objects.h>
#include <pbkit/pbkit.h>
#include "GL/gl.h"
#include "pbgl.h"
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#ifdef NXDK
#include "networking.hpp"

// clang-format off
#ifdef FC_USE_SDL_GPU
#include "nxdk-sdl-gpu/nxdkSDLGPU.h"
#endif // FC_USE_SDL_GPU
// clang-format on

void mountHomeDir(const char Letter) {
  char targetPath[MAX_PATH];
  char* finalSeparator;
  nxGetCurrentXbeNtPath(targetPath);

  finalSeparator = strrchr(targetPath, '\\');
  *(finalSeparator + 1) = '\0';
  nxMountDrive(Letter, targetPath);
}
#endif

int init_systems(const Config& config) {
#ifdef NXDK
  VIDEO_MODE xmode;
  void* p = NULL;
  while (XVideoListModes(&xmode, 0, 0, &p)) {
  }
  xmode.bpp = 16;
  XVideoSetMode(xmode.width, xmode.height, xmode.bpp, xmode.refresh);
  xmode = XVideoGetMode();
  InfoLog::outputLine(InfoLog::DEBUG, "Video set to %dx%d %dbpp @%dHz\n", xmode.width,
                      xmode.height, xmode.bpp, xmode.refresh);

  if (!nxMountDrive('C', "\\Device\\Harddisk0\\Partition2")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting error: Could not mount drive C\n");
  }
  if (!nxMountDrive('E', "\\Device\\Harddisk0\\Partition1")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting error: Could not mount drive E\n");
  }
  if (config.settings.mount.getFEnabled()
      && !nxMountDrive('F', "\\Device\\Harddisk0\\Partition6")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting warning: Could not mount drive F\n");
  }
  if (config.settings.mount.getGEnabled()
      && !nxMountDrive('G', "\\Device\\Harddisk0\\Partition7")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting information: Drive G:\\ not mounted\n");
  }
  if (!nxMountDrive('X', "\\Device\\Harddisk0\\Partition3")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting error: Could not mount drive X\n");
  }
  if (!nxMountDrive('Y', "\\Device\\Harddisk0\\Partition4")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting error: Could not mount drive Y\n");
  }
  if (!nxMountDrive('Z', "\\Device\\Harddisk0\\Partition5")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting error: Could not mount drive Z\n");
  }
  if (nxIsDriveMounted('D')) {
    nxUnmountDrive('D');
  }
  if (!nxMountDrive('D', "\\Device\\CdRom0")) {
    InfoLog::outputLine(InfoLog::ERROR, "Mounting warning: Could not mount DVD drive\n");
  }

  pb_set_color_format(NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5, false);
  int status = pbgl_init(GL_TRUE);
  if (status) {
    InfoLog::outputLine(InfoLog::ERROR, "pbgl_init error: %d", status);
    return 4;
  }

  pbgl_set_swap_interval(1);
#endif

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    InfoLog::outputLine(InfoLog::ERROR, "SDL_Init error: %s", SDL_GetError());
    return 3;
  }

#ifdef NXDK
#ifdef FC_USE_SDL_GPU
  pbglConfigureSDLVideoDevice();
#endif
#endif

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    InfoLog::outputLine(InfoLog::ERROR, "IMG Init Error: %s!\n", IMG_GetError());
    return 2;
  }

  if (TTF_Init() != 0) {
    InfoLog::outputLine(InfoLog::ERROR, "TTF Init Error: %s", TTF_GetError());
    return 2;
  }

  if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
    InfoLog::outputLine(InfoLog::ERROR, "SDL_INIT_GAMECONTROLLER error: %s", SDL_GetError());
    return 3;
  }
  return 0;
}

void shutdown_systems(int systems) {
#ifdef NXDK
  if (systems <= 1) {
    closeNetwork();
  }
#endif
  if (systems <= 2) {
    IMG_Quit();
    TTF_Quit();
  }
  if (systems <= 3) {
    SDL_Quit();
  }
#ifdef NXDK
  XReboot();
#endif
}
