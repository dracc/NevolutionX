#include "menuItem.h"

menuItem::menuItem(std::string const& text) {
  label = text;
  texture = nullptr;
}

menuItem::menuItem(const menuItem& itm) :
  menuItem(itm.getLabel()) {
  this->setTexture(itm.getTexture());
}

menuItem::~menuItem() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
}

std::string menuItem::getLabel() const {
  return label;
}

void menuItem::setLabel(std::string const& text) {
  label = text;
}

SDL_Texture* menuItem::getTexture() const{
  return texture;
}

void menuItem::setTexture(SDL_Texture* t) {
  texture = t;
}
