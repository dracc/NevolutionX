#ifndef FONT_H
#define FONT_H

#include <string>
#include <utility>
#include <vector>
#include "renderer.hpp"

// clang-format off
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#include "3rdparty/SDL_FontCache/SDL_FontCache.h"
#pragma clang diagnostic pop
// clang-format on

class Font {
private:
  FC_Font* fcFont;
  Renderer& renderer;

public:
  Font(Renderer& renderer, const char* path);
  ~Font();

  std::pair<float, float> draw(const std::string& str, std::pair<float, float> coordinates);
  std::pair<float, float> drawColumn(const std::string& str,
                                     std::pair<float, float> coordinates,
                                     float maxWidth);

  float getFontHeight() const;
  float getColumnHeight(const std::string& str, float maxWidth) const;
  float getTextHeight(const std::string& str) const;
  float getTextWidth(const std::string& str) const;
};

#endif
