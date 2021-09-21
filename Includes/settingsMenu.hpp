#ifndef __SETTINGSMENU_H
#define __SETTINGSMENU_H

#include <string>
#include <vector>
#include "menu.hpp"

class switchingMenuItem : public MenuItem {
protected:
  std::vector<std::string> values;
  std::string valuedLabel;
  int selected;

public:
  switchingMenuItem(std::string const& label, std::vector<std::string>);
  std::string_view getLabel() const;
};

class togglingEEPROMMenuItem : public MenuItem {
  bool value = false;
  std::string valuedLabel;
  uint32_t bitmask;
  uint32_t valueIndex;

public:
  togglingEEPROMMenuItem(std::string const& label, uint32_t vI, uint32_t bmask);
  std::string_view getLabel() const;
  void execute(Menu*);
};

class settingsMenu : public MenuNode {
public:
  settingsMenu(MenuNode* parent, std::string const& label);

private:
  void init();
};


#endif
