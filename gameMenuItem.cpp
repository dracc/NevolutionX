#include "gameMenuItem.h"

gameMenuItem::gameMenuItem(char* text, char* p) :
  menuItem(text) {
  xbePath = (char*)malloc(strlen(p) * sizeof(char) + 1);
  strcpy(xbePath, p);
}

gameMenuItem::gameMenuItem(const char* text, const char* p) :
  gameMenuItem(const_cast<char*>(text), const_cast<char*>(p)) {}

gameMenuItem::~gameMenuItem() {
  if (getTexture() != nullptr) {
    SDL_DestroyTexture(getTexture());
    setTexture(nullptr);
  }
}

char* gameMenuItem::getXBEPath() {
  return xbePath;
}

void gameMenuItem::setXBEPath(char* p) {
  xbePath = p;
}
