#ifndef __MEMORYMENU_HPP
#define __MEMORYMENU_HPP

#include "menu.hpp"
#include <string>

class gameSaveData : public MenuItem {
  size_t size;
  const std::string path;
  // xbx_texture *icon;
public:
  gameSaveData(MenuNode *parent, std::string const& label, std::string const& path);
  // std::string_view getLabel() const;
  void execute(Menu *);
};

class gameSave : public MenuNode {
  // xbx_texture *icon;
public:
  gameSave(MenuNode *parent, std::string const& label, std::string const& path);
};

class gameSaveMenu : public MenuNode {
public:
  gameSaveMenu(MenuNode *parent, std::string const& label);
};

#endif
