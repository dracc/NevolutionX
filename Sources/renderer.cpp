#include "renderer.hpp"
#include <SDL_image.h>
#include <algorithm>
#include <cmath>
#include "infoLog.hpp"

#ifdef NXDK
#include <hal/video.h>
#include "nxdk-sdl-gpu/nxdkSDLGPU.h"
#endif

// One line of text with the default font is 31 pixels high.
// FIXME: Should probably be dynamic and dependent of font settings.
const unsigned int FONT_TEX_SIZE = 31;

Renderer::Renderer() {
#ifdef NXDK
  VIDEO_MODE xmode = XVideoGetMode();
  height = xmode.height;
  width = xmode.width;
  windowFlags = SDL_WINDOW_SHOWN;
#ifdef FC_USE_SDL_GPU
  windowFlags |= SDL_WINDOW_OPENGL;
#endif
#else
  height = 480;
  width = 640;
  windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  // renderFlags = SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_SOFTWARE;
#endif
  overscanCompX = width * 0.075;
  overscanCompY = height * 0.075;
  menuItemCount = (height - (overscanCompY * 2)) / FONT_TEX_SIZE;
  lowerHalf = menuItemCount / 2;
  upperHalf = ceil(menuItemCount / 2.0);
}

Renderer::~Renderer() {
  if (background != nullptr) {
#ifdef FC_USE_SDL_GPU
    GPU_FreeImage(background);
#else
    SDL_DestroyTexture(background);
#endif
  }
  if (renderer != nullptr) {
#ifdef FC_USE_SDL_GPU
    GPU_FreeTarget(renderer);
#else
    SDL_DestroyRenderer(renderer);
#endif
  }
#ifndef FC_USE_SDL_GPU
  if (window != nullptr) {
    SDL_DestroyWindow(window);
  }
#endif
}

int Renderer::init() {
  window = SDL_CreateWindow("NevolutionX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            width, height, windowFlags);
  if (window == nullptr) {
    return 1;
  }

#ifdef FC_USE_SDL_GPU
  GPU_SetInitWindow(SDL_GetWindowID(window));
  renderer = GPU_Init(width, height, GPU_DEFAULT_INIT_FLAGS);
  if (!renderer) {
    return 1;
  }
#else
  renderer = SDL_CreateRenderer(window, -1, renderFlags);
#endif
  if (renderer == nullptr) {
    return 2;
  }
#ifdef FC_USE_SDL_GPU
  GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
#else
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
#endif
  setDrawColor();
  clear();
  return 0;
}

int Renderer::init(const char* bgpath) {
  int ret = init();
  if (ret != 0) {
    return ret;
  }
  char* bgname = (char*)malloc(strlen(bgpath) + 10);
  sprintf(bgname, "%s%d.png", bgpath, height);
  SDL_Surface* bgsurf = IMG_Load(bgname);
  free(bgname);
  if (bgsurf == nullptr) {
    InfoLog::outputLine(InfoLog::ERROR, "Creating background surface failed.\n");
    return 3;
  }
#ifdef FC_USE_SDL_GPU
  // OpenGL wants the surface in BGR format, but pbgl does not currently implement a
  // conversion. Since IMG_Load is only used in this one place, it is converted here
  // manually.
  auto srcFormat = static_cast<SDL_PixelFormatEnum>(bgsurf->format->format);
  switch (srcFormat) {
  case SDL_PIXELFORMAT_RGB24:
  case SDL_PIXELFORMAT_RGB888:
  case SDL_PIXELFORMAT_RGBA32:
  case SDL_PIXELFORMAT_RGBA8888:
    bgsurf = convertRGBToBGR(bgsurf);
    break;
  default:
    // Ignore surfaces that may already be BGR.
    break;
  }
  background = GPU_CopyImageFromSurface(bgsurf);
#else
  background = SDL_CreateTextureFromSurface(renderer, bgsurf);
#endif
  SDL_FreeSurface(bgsurf);
  if (background == nullptr) {
    InfoLog::outputLine(InfoLog::ERROR, "Creating background texture failed.\n");
    return 4;
  }
  return ret;
}

int Renderer::clear() {
#ifdef FC_USE_SDL_GPU
  GPU_ClearColor(renderer, drawColor);
  return 0;
#else
  int ret = SDL_RenderClear(renderer);
  return ret;
#endif
}

void Renderer::flip() {
#ifdef FC_USE_SDL_GPU
  GPU_Flip(renderer);
#else
  setDrawColor(0, 0, 0, 0xFF);
  SDL_RenderDrawRect(renderer, nullptr);
  setDrawColor();
  SDL_RenderPresent(renderer);
#endif
}

int Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#ifdef FC_USE_SDL_GPU
  drawColor.r = r;
  drawColor.g = g;
  drawColor.b = b;
  drawColor.a = a;
  return 0;
#else
  return SDL_SetRenderDrawColor(renderer, r, g, b, a);
#endif
}

void Renderer::drawTexture(NX_Texture* tex, NX_Rect& src, NX_Rect& dst) {
#ifdef FC_USE_SDL_GPU
  GPU_BlitRect(tex, &src, renderer, &dst);
#else
  SDL_RenderCopy(renderer, tex, &src, &dst);
#endif
}

void Renderer::drawTexture(NX_Texture* tex, NX_Rect& dst) {
#ifdef FC_USE_SDL_GPU
  GPU_BlitRect(tex, nullptr, renderer, &dst);
#else
  SDL_RenderCopy(renderer, tex, nullptr, &dst);
#endif
}

void Renderer::drawTexture(NX_Texture* tex, int x, int y) {
#ifdef FC_USE_SDL_GPU
  NX_Rect dst = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(tex->w),
                  static_cast<float>(tex->h) };
#else
  NX_Rect dst = { x, y, 0, 0 };
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
#endif
  drawTexture(tex, dst);
}

void Renderer::fillRectangle(const NX_Rect& dst) {
#ifdef FC_USE_SDL_GPU
  GPU_RectangleFilled2(renderer, dst, drawColor);
#else
  SDL_RenderFillRect(renderer, &dst);
#endif
}

void Renderer::fillRectangle(const SDL_FRect& dst) {
#ifdef FC_USE_SDL_GPU
  GPU_Rect rect = { dst.x, dst.y, dst.w, dst.h };
  GPU_RectangleFilled2(renderer, rect, drawColor);
#else
  SDL_RenderFillRectF(renderer, &dst);
#endif
}

void Renderer::blitSurface(SDL_Surface* bg, SDL_Surface* fg, int offset) {
  SDL_Rect dst = { offset, offset, fg->w, fg->h };
  SDL_SetSurfaceBlendMode(fg, SDL_BLENDMODE_BLEND);
  SDL_BlitSurface(fg, NULL, bg, &dst);
}

void Renderer::drawBackground() {
  if (background != nullptr) {
    drawTexture(background, 0, 0);
  }
}
