#ifndef XBEMENUITEM_H
#define XBEMENUITEM_H

#include "menuItem.h"

class xbeMenuItem: public menuItem {
private:
  char* xbePath;
public:
  xbeMenuItem(char* text, char* p);
  xbeMenuItem(const char* text, const char* p);
  xbeMenuItem(xbeMenuItem const& item);
  ~xbeMenuItem();
  const char* getXBEPath() const;
  void setXBEPath(const char* p);
  bool operator<(xbeMenuItem const& rhs) const;
};

#endif
