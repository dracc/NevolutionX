#include "menuItem.h"

menuItem::menuItem(char* text) {
  label = (char*)malloc(strlen(text) * sizeof(char) + 1);
  strcpy(label, text);
  texture = nullptr;
}

menuItem::menuItem(const char* text) :
  menuItem(const_cast<char*>(text)) {
}

menuItem::~menuItem() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
}

const char* menuItem::getLabel() {
  return label;
}

void menuItem::setLabel(char* text) {
  label = (char*)realloc(label, strlen(text) * sizeof(char) + 1);
  strcpy(label, text);
}

SDL_Texture* menuItem::getTexture() {
  return texture;
}

void menuItem::setTexture(SDL_Texture* t) {
  texture = t;
}
