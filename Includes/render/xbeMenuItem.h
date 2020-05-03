#ifndef XBEMENUITEM_H
#define XBEMENUITEM_H

#include <string>
#include "menuItem.h"

class xbeMenuItem: public menuItem {
private:
  std::string xbePath;
public:
  xbeMenuItem(std::string const& text, std::string const& p);
  xbeMenuItem(xbeMenuItem const& item);
  std::string getXBEPath() const;
  void setXBEPath(std::string const& p);
  bool operator<(xbeMenuItem const& rhs) const;
};

#endif
