#include "menu.hpp"

#include <iostream>

#include "outputLine.h"
#include "findXBE.h"
#ifdef NXDK
#include <hal/xbox.h>
#endif


/******************************************************************************************
                                   MenuItem
******************************************************************************************/
MenuItem::MenuItem(std::string const& label) : label(label) {

}

MenuItem::MenuItem(MenuNode *parent, std::string const& label) :
  parentNode(parent), label(label) {

}

MenuItem::~MenuItem() {

}

std::string_view MenuItem::getLabel() const {
  return this->label;
}

MenuNode *MenuItem::getParent() const {
  return parentNode;
}

void MenuItem::setParent(MenuNode* parent) {
  parentNode = parent;
}

/******************************************************************************************
                                   MenuNode
******************************************************************************************/

MenuNode::MenuNode(std::string const& label) : MenuItem(label) {

}

MenuNode::MenuNode(MenuNode *parent, std::string const& label) :
  MenuItem(label) {
  this->setParent(parent);
}

MenuNode::~MenuNode() {

}

void MenuNode::execute(Menu *menu) {
  if (menu->getCurrentMenu() != this) {
    menu->setCurrentMenu(this);
  }
  else {
    if (childNodes.size() > selected) {
      this->childNodes.at(selected)->execute(menu);
    }
  }
}

size_t MenuNode::getSelected() {
  return this->selected;
}

std::vector<std::shared_ptr<MenuItem>> *MenuNode::getChildNodes() {
  return &this->childNodes;
}

void MenuNode::addNode(std::shared_ptr<MenuItem> node) {
  this->childNodes.push_back(node);
}

void MenuNode::up() {
  if (selected == 0) {
    selected = childNodes.size() - 1;
  } else {
    --selected;
  }
}

void MenuNode::down() {
  selected = (selected + 1) % childNodes.size();
}

/******************************************************************************************
                                   MenuXbe
******************************************************************************************/
MenuXbe::MenuXbe(MenuNode *parent, std::string const& label, std::string const& path) :
  MenuNode(parent, label), path(path) {
  // Find "default.xbe"'s and add them to ChildNodes
  findXBE(path, this);
  std::sort(begin(childNodes), end(childNodes),
            [](std::shared_ptr<MenuItem> a,
               std::shared_ptr<MenuItem> b){ return a->getLabel() < b->getLabel();});
}

MenuXbe::~MenuXbe() {

}

void MenuXbe::execute(Menu *menu) {
  if (menu->getCurrentMenu() != this) {
    menu->setCurrentMenu(this);
  }
  else {
    if (childNodes.size() > selected) {
      this->childNodes.at(selected)->execute(menu);
    }
  }
}

/******************************************************************************************
                                   MenuLaunch
******************************************************************************************/
MenuLaunch::MenuLaunch(std::string const& label, std::string const& path) :
  MenuItem(label), path(path) {
  // Find "default.xbe"'s and add them to ChildNodes
}

MenuLaunch::~MenuLaunch() {

}

void MenuLaunch::execute(Menu *) {
  outputLine("Launching xbe %s\n", this->path.c_str());
#ifdef NXDK
  XLaunchXBE(const_cast<char*>(this->path.c_str()));
#endif
}

/******************************************************************************************
                                   Menu
******************************************************************************************/
Menu::Menu(const Config &config, Renderer &renderer) : renderer(renderer), rootNode("root") {
  rootNode.setParent(&rootNode);
  currentMenu = &rootNode;
  menuHeight = renderer.getHeight() * 0.9;
  startHeight = renderer.getHeight() * 0.1;
  for (auto &e : config.json["menu"]) {
    if (!static_cast<std::string>(e["type"]).compare("scan")) {
      std::shared_ptr<MenuXbe> newNode = std::make_shared<MenuXbe>(currentMenu, e["label"], e["path"]);
      this->rootNode.addNode(newNode);
    }
    else if (!static_cast<std::string>(e["type"]).compare("launch")) {
      std::shared_ptr<MenuLaunch> newNode = std::make_shared<MenuLaunch>(e["label"], e["path"]);
      this->rootNode.addNode(newNode);
    }
    else if (!static_cast<std::string>(e["type"]).compare("reboot")) {
      std::shared_ptr<MenuNode> newNode = std::make_shared<MenuNode>(currentMenu, e["label"]);
      this->rootNode.addNode(newNode);
    }
    else if (!static_cast<std::string>(e["type"]).compare("settings")) {
      std::shared_ptr<MenuNode> newNode = std::make_shared<MenuNode>(currentMenu, e["label"]);
      this->rootNode.addNode(newNode);
    }
  }
}

void Menu::render(Font &font) {
  if (this->currentMenu->getChildNodes()->empty()) {
    return;
  }
  std::pair<float, float> coordinates(100, startHeight);
  std::string menutext;
  size_t i = 0;
  for (auto menuNode : *this->currentMenu->getChildNodes()) {
    menutext = std::string(menuNode->getLabel());
    std::pair<float, float> dimensions;
    dimensions = font.draw(menutext, coordinates);

    if (i == this->currentMenu->getSelected()) {
      SDL_Rect rect;
      rect.w = std::get<0>(dimensions) + 20;
      rect.h = std::get<1>(dimensions);
      rect.x = std::get<0>(coordinates) - 10;
      rect.y = std::get<1>(coordinates);
      renderer.setDrawColor(0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderDrawRect(renderer.getRenderer(), &rect);
    }

    coordinates = std::pair<float, float>(std::get<0>(coordinates),
                                          std::get<1>(coordinates) + std::get<1>(dimensions));
    if (std::get<1>(coordinates) > menuHeight) {
      break;
    }
    ++i;
  }
}

MenuNode *Menu::getCurrentMenu() {
  return currentMenu;
}

void Menu::setCurrentMenu(MenuNode *menu) {
  currentMenu = menu;
}

void Menu::up() {
  currentMenu->up();
}

void Menu::down() {
  currentMenu->down();
}

void Menu::back() {
  outputLine("Setting menu to %s\n", std::string(currentMenu->getParent()->getLabel()).c_str());
  currentMenu = currentMenu->getParent();
}

void Menu::execute() {
  this->currentMenu->execute(this);
}
