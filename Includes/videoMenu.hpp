#ifndef __VIDEOMENU_HPP
#define __VIDEOMENU_HPP

#include "menu.hpp"
#include "settingsMenu.hpp"
#include <string>

class videoRatioMenuItem : public switchingMenuItem {
private:
  uint32_t valueIndex = 0x8;
public:
  videoRatioMenuItem();
  void execute(Menu *);
};

class videoMenu : public MenuNode {
public:
  videoMenu(MenuNode *parent, std::string const& label);
};

#endif
