#ifndef FONT_H
#define FONT_H

#include "xbeMenuItem.h"
#include "renderer.h"
#include <vector>

#include <SDL.h>
#include "SDL_ttf.h"

#include "3rdparty/SDL_FontCache/SDL_FontCache.h"

class Font {
private:
  bool textureHelper(menuItem* mI, SDL_Color const& c, Renderer* r);
  TTF_Font* font;
  TTF_Font* outline_font;
  SDL_Color active, passive, outline_color;
  int outline_size = 1;
  FC_Font *fcFont;
  Renderer &renderer;
public:
  Font(Renderer &renderer, const char* path);
  ~Font();

  std::pair<float, float> draw(const std::string &str, std::pair<float, float> coordinates);

  size_t createTextures(std::vector<menuItem> &items, Renderer* r);
  size_t createTextures(std::vector<xbeMenuItem> &items, Renderer* r);

  size_t setActive(menuItem* item, Renderer* r);
  size_t setPassive(menuItem* item, Renderer* r);
};

#endif
