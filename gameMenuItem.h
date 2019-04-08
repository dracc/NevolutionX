#ifndef GAMEMENUITEM_H
#define GAMEMENUITEM_H

#include "menuItem.h"

class gameMenuItem: public menuItem {
private:
  char* xbePath;
public:
  gameMenuItem(char* text, char* p);
  gameMenuItem(const char* text, const char* p);
  ~gameMenuItem();
  char* getXBEPath();
  void setXBEPath(char* p);
};

#endif
