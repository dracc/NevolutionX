#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
#include <SDL.h>

class menuItem {
private:
  SDL_Texture* texture;
  std::string label;
public:
  menuItem(std::string const& text);
  menuItem(const menuItem& itm);
  ~menuItem();

  std::string getLabel() const;
  void setLabel(std::string const& text);

  SDL_Texture* getTexture() const;
  void setTexture(SDL_Texture* t);
};
#endif
