#include "menu.hpp"

#include <iostream>

#include "outputLine.h"
#ifdef NXDK
#include <hal/xbox.h>
#endif


/******************************************************************************************
                                   MenuItem
******************************************************************************************/
MenuItem::MenuItem(std::string const& label) : label(label) {

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

MenuNode::~MenuNode() {

}

void MenuNode::execute() {
  outputLine("%d", selected);
  this->childNodes.at(selected)->execute();
}

size_t MenuNode::getSelected() {
  return this->selected;
}

std::vector<MenuItem*> *MenuNode::getChildNodes() {
  return &this->childNodes;
}

void MenuNode::addNode(MenuItem *node) {
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
MenuXbe::MenuXbe(std::string const& label, std::string const& path) :
  MenuNode(label), path(path) {
  // Find "default.xbe"'s and add them to ChildNodes
}

MenuXbe::~MenuXbe() {

}

void MenuXbe::execute() {

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

void MenuLaunch::execute() {
  outputLine("Launching xbe %s", this->path.c_str());
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
  for (auto &e : config.json["menu"]) {
    if (!static_cast<std::string>(e["type"]).compare("scan")) {
      MenuNode *newNode = new MenuXbe(e["label"], e["path"]);
      this->rootNode.addNode(newNode);
    }
    else if (!static_cast<std::string>(e["type"]).compare("launch")) {
      MenuLaunch *newNode = new MenuLaunch(e["label"], e["path"]);
      this->rootNode.addNode(newNode);
    }
    else if (!static_cast<std::string>(e["type"]).compare("reboot")) {
      MenuNode *newNode = new MenuNode(e["label"]);
      this->rootNode.addNode(newNode);
    }
    else if (!static_cast<std::string>(e["type"]).compare("settings")) {
      MenuNode *newNode = new MenuNode(e["label"]);
      this->rootNode.addNode(newNode);
    }
  }
}

void Menu::render(Font &font) {
  std::pair<float, float> coordinates(100, 100);
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

    ++i;
  }
}

MenuNode *Menu::getCurrentMenu() {
  return currentMenu;
}

void Menu::up() {
  currentMenu->up();
}

void Menu::down() {
  currentMenu->down();
}

void Menu::back() {
  outputLine("Setting menu to %s", std::string(currentMenu->getParent()->getLabel()).c_str());
  currentMenu = currentMenu->getParent();
}
