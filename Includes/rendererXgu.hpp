#ifndef RENDERERXGU_H
#define RENDERERXGU_H

#include <xgu.h>
#include <xgux.h>
#include <vector>

typedef struct Texture {
  uint16_t width;
  uint16_t height;
  uint16_t pitch;
  void* addr;
} Texture;

class RendererXgu {
public:
  RendererXgu();
  ~RendererXgu();

  int init();
  int init(const char* bg);
  void clear();
  void flip();

  SDL_Renderer* getRenderer() { return renderer; }
  int getWidth() const { return width; }
  int getHeight() const { return height; }

  int setDrawColor(uint8_t r = 0x40, uint8_t g = 0x40, uint8_t b = 0xE0, uint8_t a = 0x00);

  void drawTexture(SDL_Texture* tex, SDL_Rect& src, SDL_Rect& dst);
  void drawTexture(SDL_Texture* tex, SDL_Rect& dst);
  void drawTexture(SDL_Texture* tex, int x, int y);

  void fillRectangle(const SDL_Rect& dst);
  void fillRectangle(const SDL_FRect& dst);

  void blitSurface(SDL_Surface* bg, SDL_Surface* fg, int offset);

  void drawBackground();
  void initTexture(std::string const& path);

private:
  Texture* background = nullptr;

  uint32_t color;
  int height = 0;
  int width = 0;
  int overscanCompX = 0;
  int overscanCompY = 0;
  size_t menuItemCount = 0;
  size_t lowerHalf = 0;
  size_t upperHalf = 0;
};

#endif
