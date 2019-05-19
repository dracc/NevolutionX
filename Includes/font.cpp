#include "font.h"

Font::Font(const char* path) {
  font = TTF_OpenFont(path, 24);
  outline_font = TTF_OpenFont(path, 24);
  active = {0x7F, 0x7F, 0xFF, 0xFF};
  passive = {0xFF, 0x7F, 0xFF, 0xFF};
  outline_color = {0x40, 0x40, 0x40, 0x7F};
  TTF_SetFontOutline(outline_font, outline_size);
}

Font::~Font() {
  if (font != nullptr) {
    TTF_CloseFont(font);
  }
  if (outline_font != nullptr) {
    TTF_CloseFont(outline_font);
  }
}

bool Font::textureHelper(menuItem* mI, SDL_Color const& c, Renderer* r) {
  if (mI == nullptr) {
    return false;
  }
  if (mI->getTexture() != nullptr) {
    SDL_DestroyTexture(mI->getTexture());
  }
  SDL_Surface *bgSurf = TTF_RenderText_Blended(outline_font, mI->getLabel(),
                                               outline_color);
  SDL_Surface *fgSurf = TTF_RenderText_Blended(font, mI->getLabel(), c);
  if (bgSurf == nullptr || fgSurf == nullptr) {
    return false;
  }
  r->blitSurface(bgSurf, fgSurf, outline_size);
  mI->setTexture(r->surfaceToTexture(bgSurf));
  if (mI->getTexture() == nullptr) {
    return false;
  }
  SDL_FreeSurface(bgSurf);
  SDL_FreeSurface(fgSurf);
  return true;
}

size_t Font::createTextures(vector<menuItem> &items, Renderer* r) {
  if (r->getRenderer() == nullptr || items.empty()) {
    return 0;
  }
  for (size_t i = 0; i < items.size(); ++i) {
    SDL_Color color = (i == 0 ? active : passive);
    if (!textureHelper(&items[i], color, r)) {
      return i;
    }
  }
  return items.size();
}

// FIXME: This function should not be necessary.
size_t Font::createTextures(vector<xbeMenuItem> &items, Renderer* r) {
  if (r->getRenderer() == nullptr || items.empty()) {
    return 0;
  }
  for (size_t i = 0; i < items.size(); ++i) {
    SDL_Color color = (i == 0 ? active : passive);
    if (!textureHelper(&items[i], color, r)) {
      return i;
    }
  }
  return items.size();
}

size_t Font::setActive(menuItem *item, Renderer *r) {
  if (!textureHelper(item, active, r)) {
    return 0;
  }
  return 1;
}

size_t Font::setPassive(menuItem *item, Renderer *r) {
  if (!textureHelper(item, passive, r)) {
    return 0;
  }
  return 1;
}
