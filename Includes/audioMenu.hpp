#ifndef __AUDIOMENU_HPP
#define __AUDIOMENU_HPP

#include <string>
#include "menu.hpp"
#include "settingsMenu.hpp"

class audioChannelsMenuItem : public switchingMenuItem {
private:
  uint32_t valueIndex = 0x9;

public:
  audioChannelsMenuItem();
  void execute(Menu*);
};

class audioMenu : public MenuNode {
public:
  audioMenu(MenuNode* parent, std::string const& label);
};

#endif
