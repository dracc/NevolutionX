#ifndef RENDERER_H
#define RENDERER_H

#ifdef NXDK
#include <hal/video.h>
#endif

#include <SDL.h>
#include "xbeMenuItem.h"
#include "menuItem.h"
#include "vector.hpp"

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

  SDL_Renderer* getRenderer() {return renderer;}

  int setDrawColor(uint8_t r = 0x40, uint8_t g = 0x40,
                   uint8_t b = 0xE0, uint8_t a = 0x00);

  void drawTexture(SDL_Texture* tex, SDL_Rect &src, SDL_Rect &dst);
  void drawTexture(SDL_Texture* tex, SDL_Rect &dst);
  void drawTexture(SDL_Texture* tex, int x, int y);

  void drawBackground();
  void drawMenuTexture(SDL_Texture* tex, int numItems, int currItem);
  void drawMenuTexture(SDL_Texture* tex);

  SDL_Texture* surfaceToTexture(SDL_Surface* surf);

  void destroyTexture(SDL_Texture* tex);

  SDL_Texture* compileList(vector<xbeMenuItem> &l, size_t currItem);
  SDL_Texture* compileList(vector<xbeMenuItem> &l);
  SDL_Texture* compileList(vector<menuItem> &l);

private:
  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;
  SDL_Texture *background = nullptr;
  Uint32 renderFlags = 0;
  Uint32 windowFlags = 0;
#ifndef NXDK
  int SCREEN_HEIGHT = 0;
  int SCREEN_WIDTH = 0;
#endif
  int overscanCompX = 0;
  int overscanCompY = 0;
};

#endif
