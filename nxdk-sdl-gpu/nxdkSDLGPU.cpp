#include "nxdkSDLGPU.h"
#include <SDL.h>
#include "infoLog.hpp"
#include "pbgl.h"

// clang-format off
extern "C" {
#include <SDL2/src/video/SDL_sysvideo.h>
};
// clang-format on

extern "C" {

static int PBGL_LoadLibrary(_THIS, const char*) {
  // XBOX/pbgl does not support dynamic libraries.
  (void)_this;
  return 0;
}

static void* PBGL_GetProcAddress(_THIS, const char*) {
  (void)_this;
  return nullptr;
}

static void PBGL_UnloadLibrary(_THIS) {
  // XBOX/pbgl does not support dynamic libraries.
  (void)_this;
}

static SDL_GLContext PBGL_CreateContext(_THIS, SDL_Window*) {
  (void)_this;
  return reinterpret_cast<void*>(0xDEADBEEF);
}

static int PBGL_MakeCurrent(_THIS, SDL_Window*, SDL_GLContext) {
  (void)_this;
  return 0;
}

static void PBGL_GetDrawableSize(_THIS, SDL_Window* window, int* w, int* h) {
  (void)_this;
  SDL_GetWindowSize(window, w, h);
}

static int PBGL_SetSwapInterval(_THIS, int interval) {
  (void)_this;
  if (interval < 0) {
    return -1;
  }
  pbgl_set_swap_interval(interval);
  return 0;
}

static int PBGL_GetSwapInterval(_THIS) {
  (void)_this;
  return pbgl_get_swap_interval();
}

static int PBGL_SwapWindow(_THIS, SDL_Window*) {
  (void)_this;
  pbgl_swap_buffers();
  return 0;
}

static void PBGL_DeleteContext(_THIS, SDL_GLContext) {
  // PBGL has no context, so nothing needs to be done.
  (void)_this;
}

static void GL_DefaultProfileConfig(_THIS, int* mask, int* major, int* minor) {
  (void)_this;
  *mask = 0;
  *major = 1;
  *minor = 1;
}

}; // extern "C"

void pbglConfigureSDLVideoDevice() {
  SDL_VideoDevice* device = SDL_GetVideoDevice();
  if (!device) {
    InfoLog::outputLine(InfoLog::ERROR, "No SDL video device!\n");
    return;
  }

  device->GL_LoadLibrary = PBGL_LoadLibrary;
  device->GL_GetProcAddress = PBGL_GetProcAddress;
  device->GL_UnloadLibrary = PBGL_UnloadLibrary;
  device->GL_CreateContext = PBGL_CreateContext;
  device->GL_MakeCurrent = PBGL_MakeCurrent;
  device->GL_GetDrawableSize = PBGL_GetDrawableSize;
  device->GL_SetSwapInterval = PBGL_SetSwapInterval;
  device->GL_GetSwapInterval = PBGL_GetSwapInterval;
  device->GL_SwapWindow = PBGL_SwapWindow;
  device->GL_DeleteContext = PBGL_DeleteContext;
  device->GL_DefaultProfileConfig = GL_DefaultProfileConfig;
}


SDL_Surface* convertRGBToBGR(SDL_Surface* src) {
  for (int32_t y = 0; y < src->h; ++y) {
    uint8_t* data = reinterpret_cast<uint8_t*>(src->pixels) + y * src->pitch;
    for (int32_t x = 0; x < src->w; ++x, data += src->format->BytesPerPixel) {
      uint8_t tmp = *data;
      *data = data[2];
      data[2] = tmp;
    }
  }

  return src;
}