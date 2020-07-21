#include "font.h"

#include "outputLine.h"
#include "3rdparty/SDL_FontCache/SDL_FontCache.h"

#include <cassert>

Font::Font(Renderer &renderer, const char* path) : renderer(renderer) {
  fcFont = FC_CreateFont();
  assert(fcFont);
  bool load_success = FC_LoadFont(fcFont, renderer.getRenderer(), path,
                                  20, FC_MakeColor(120,120,150,255), TTF_STYLE_NORMAL);
  assert(load_success);
}

Font::~Font() {
  FC_FreeFont(fcFont);
}

std::pair<float, float> Font::draw(const std::string &str, std::pair<float, float> coordinates) {
  FC_Draw(fcFont, renderer.getRenderer(),
          std::get<0>(coordinates), std::get<1>(coordinates), "%s", str.c_str());
  FC_Rect rect = FC_GetBounds(fcFont,
                              std::get<0>(coordinates), std::get<1>(coordinates),
                              FC_ALIGN_LEFT, FC_MakeScale(1.0f, 1.0f), "%s", str.c_str());
  return std::pair<float, float>(rect.w, rect.h);
}
