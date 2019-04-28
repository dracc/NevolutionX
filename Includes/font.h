#ifndef FONT_H
#define FONT_H

#include "xbeMenuItem.h"
#include "renderer.h"
#include "vector.hpp"

#include <SDL.h>
#include "SDL_ttf.h"

class Font {
private:
  bool textureHelper(menuItem* mI, SDL_Color const& c, Renderer* r);
  TTF_Font* font;
  size_t texH, texW;
  SDL_Color active, passive;
public:
  Font(const char* path);
  ~Font();

  size_t createTextures(vector<menuItem> &items, Renderer* r);
  size_t createTextures(vector<xbeMenuItem> &items, Renderer* r);

  size_t setActive(menuItem* item, Renderer* r);
  size_t setPassive(menuItem* item, Renderer* r);
};

#endif
