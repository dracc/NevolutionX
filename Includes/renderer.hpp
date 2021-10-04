#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <vector>

// clang-format off
#ifdef FC_USE_SDL_GPU
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#include "SDL_gpu.h"
#pragma clang diagnostic pop
#endif
// clang-format on

#ifdef FC_USE_SDL_GPU
typedef GPU_Image NX_Texture;
typedef GPU_Rect NX_Rect;
typedef GPU_Target NX_Target;
#else
typedef SDL_Texture NX_Texture;
typedef SDL_Rect NX_Rect;
typedef SDL_Renderer NX_Target;
#endif

class Renderer {
public:
  Renderer();
  ~Renderer();

  int init();
  int init(const char* bg);
  int clear();
  void flip();

#ifdef FC_USE_SDL_GPU
  GPU_Target* getRenderer() { return renderer; }
#else
  SDL_Renderer* getRenderer() { return renderer; }
#endif
  int getWidth() const { return width; }
  int getHeight() const { return height; }

  int setDrawColor(uint8_t r = 0x40, uint8_t g = 0x40, uint8_t b = 0xE0, uint8_t a = 0x00);

  void drawTexture(NX_Texture* tex, NX_Rect& src, NX_Rect& dst);
  void drawTexture(NX_Texture* tex, NX_Rect& dst);
  void drawTexture(NX_Texture* tex, int x, int y);

  void fillRectangle(const NX_Rect& dst);
  void fillRectangle(const SDL_FRect& dst);

  void blitSurface(SDL_Surface* bg, SDL_Surface* fg, int offset);

  void drawBackground();

private:
  NX_Target* renderer = nullptr;
  NX_Texture* background = nullptr;

  SDL_Window* window = nullptr;
#ifndef FC_USE_SDL_GPU
  Uint32 renderFlags = 0;
#endif
  Uint32 windowFlags = 0;

  int height = 0;
  int width = 0;
  int overscanCompX = 0;
  int overscanCompY = 0;
  size_t menuItemCount = 0;
  size_t lowerHalf = 0;
  size_t upperHalf = 0;

#ifdef FC_USE_SDL_GPU
  SDL_Color drawColor;
#endif
};

#endif
