#include "font.h"

Font::Font(const char* path) {
  font = TTF_OpenFont(path, 24);
  active = {0x7F, 0x7F, 0xFF, 0xFF};
  passive = {0xFF, 0x7F, 0xFF, 0xFF};
}

Font::~Font() {
  if (font != nullptr) {
    TTF_CloseFont(font);
  }
}

bool Font::textureHelper(menuItem* mI, SDL_Color const& c, Renderer* r) {
  if (mI == nullptr) {
    return false;
  }
  if (mI->getTexture() != nullptr) {
    SDL_DestroyTexture(mI->getTexture());
  }
  SDL_Surface *tmpSurf = TTF_RenderText_Blended(font, mI->getLabel(), c);
  if (tmpSurf == nullptr) {
    return false;
  }
  mI->setTexture(r->surfaceToTexture(tmpSurf));
  if (mI->getTexture() == nullptr) {
    return false;
  }
  SDL_FreeSurface(tmpSurf);
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
