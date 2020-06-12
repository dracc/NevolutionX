#include <NevolutionX/menu/xbeMenuItem.h>
#include <cstring>

xbeMenuItem::xbeMenuItem(std::string const& text, std::string const& p) :
  menuItem(text) {
  xbePath = p;
}

xbeMenuItem::xbeMenuItem(xbeMenuItem const& item) :
  xbeMenuItem(item.getLabel(), item.getXBEPath()) {
  setTexture(item.getTexture());
}

std::string xbeMenuItem::getXBEPath() const {
  return xbePath;
}

void xbeMenuItem::setXBEPath(std::string const& p) {
  xbePath = p;
}

bool xbeMenuItem::operator<(xbeMenuItem const& rhs) const {
  return getLabel() < rhs.getLabel();
}
