#include "xbeMenuItem.h"

xbeMenuItem::xbeMenuItem(char* text, char* p) :
  menuItem(text) {
  xbePath = (char*)malloc(strlen(p) * sizeof(char) + 1);
  strcpy(xbePath, p);
}

xbeMenuItem::xbeMenuItem(const char* text, const char* p) :
  xbeMenuItem(const_cast<char*>(text), const_cast<char*>(p)) {}

xbeMenuItem::~xbeMenuItem() {
  if (getTexture() != nullptr) {
    SDL_DestroyTexture(getTexture());
    setTexture(nullptr);
  }
}

char* xbeMenuItem::getXBEPath() {
  return xbePath;
}

void xbeMenuItem::setXBEPath(char* p) {
  xbePath = p;
}
