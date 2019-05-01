#ifndef MENUITEM_H
#define MENUITEM_H

#include <stdlib.h>
#include <string.h>
#include <SDL.h>

class menuItem {
private:
  SDL_Texture* texture;
  char* label;
public:
  menuItem(char* text);
  menuItem(const char* text);
  menuItem(const menuItem& itm);
  ~menuItem();

  const char* getLabel() const;
  void setLabel(const char* text);

  SDL_Texture* getTexture() const;
  void setTexture(SDL_Texture* t);
};
#endif
