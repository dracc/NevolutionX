#ifndef FONT_H
#define FONT_H

#include "xbeMenuItem.h"
#include "renderer.h"
#include <vector>

#include <SDL.h>
#include "SDL_ttf.h"

class Font {
private:
  bool textureHelper(menuItem* mI, SDL_Color const& c, Renderer* r);
  TTF_Font* font;
  TTF_Font* outline_font;
  SDL_Color active, passive, outline_color;
  int outline_size = 1;
public:
  Font(const char* path);
  ~Font();

  size_t createTextures(std::vector<menuItem> &items, Renderer* r);
  size_t createTextures(std::vector<xbeMenuItem> &items, Renderer* r);

  size_t setActive(menuItem* item, Renderer* r);
  size_t setPassive(menuItem* item, Renderer* r);
};

#endif
