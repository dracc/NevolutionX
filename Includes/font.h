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
  FC_Font *fcFont;
  Renderer &renderer;
public:
  Font(Renderer &renderer, const char* path);
  ~Font();

  std::pair<float, float> draw(const std::string &str, std::pair<float, float> coordinates);
};

#endif
