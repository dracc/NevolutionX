#ifndef __SETTINGSMENU_H
#define __SETTINGSMENU_H

#include "menu.hpp"

class settingsMenu : public MenuNode {
public:
  settingsMenu(MenuNode *parent, std::string const& label);

private:
  void init();
};


#endif
