#ifndef XBEMENUITEM_H
#define XBEMENUITEM_H

#include "menuItem.h"

class xbeMenuItem: public menuItem {
private:
  char* xbePath;
public:
  xbeMenuItem(char* text, char* p);
  xbeMenuItem(const char* text, const char* p);
  ~xbeMenuItem();
  char* getXBEPath();
  void setXBEPath(char* p);
};

#endif
