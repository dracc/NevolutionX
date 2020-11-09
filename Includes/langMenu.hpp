#ifndef LANGMENU_HPP
#define LANGMENU_HPP

#include "menu.hpp"
#include <string>
#include <utility>
#include <vector>

class LangItem : public MenuItem {
public:
  LangItem(MenuNode *parent, std::string const& label, uint32_t langnum);

  void execute(Menu *menu);

private:
  uint32_t langnum;
};

class LangMenu : public MenuNode {
public:
  LangMenu(MenuNode *parent, std::string const& label);
};

#endif
