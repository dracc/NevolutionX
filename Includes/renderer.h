#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <vector>

int min(int lhs, int rhs);
int max(int lhs, int rhs);

class Renderer {
public:
  Renderer();
  ~Renderer();

  int init();
  int init(const char* bg);
  int clear();
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

private:
  SDL_Renderer* renderer = nullptr;
  SDL_Window* window = nullptr;
  SDL_Texture* background = nullptr;
  Uint32 renderFlags = 0;
  Uint32 windowFlags = 0;

  int height = 0;
  int width = 0;
  int overscanCompX = 0;
  int overscanCompY = 0;
  size_t menuItemCount = 0;
  size_t lowerHalf = 0;
  size_t upperHalf = 0;
};

#endif
