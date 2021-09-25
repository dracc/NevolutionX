#include "menu.hpp"
#include "3rdparty/NaturalSort/natural_sort.hpp"
#include "infoLog.h"
#include "settingsMenu.hpp"
#include "xbeScanner.h"
#ifdef NXDK
#include <hal/xbox.h>
#endif

// Character used in the config.json to separate multiple path entries.
#define PATH_DELIMITER ','

/******************************************************************************************
                                   MenuItem
******************************************************************************************/
MenuItem::MenuItem(std::string const& label) : label(label) {
}

MenuItem::MenuItem(MenuNode* parent, std::string const& label) :
    parentNode(parent), label(label) {
}

MenuItem::~MenuItem() {
}

std::string_view MenuItem::getLabel() const {
  return this->label;
}

MenuNode* MenuItem::getParent() const {
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

MenuNode::MenuNode(MenuNode* parent, std::string const& label) : MenuItem(label) {
  this->setParent(parent);
}

MenuNode::~MenuNode() {
}

void MenuNode::execute(Menu* menu) {
  if (menu->getCurrentMenu() != this) {
    menu->setCurrentMenu(this);
  } else {
    if (childNodes.size() == 0) {
      return;
    }
    if (childNodes.size() > selected) {
      this->childNodes.at(selected)->execute(menu);
    }
  }
}

void MenuNode::setSelected(size_t id) {
  if (id < childNodes.size()) {
    selected = id;
  }
}

size_t MenuNode::getSelected() {
  return this->selected;
}

std::vector<std::shared_ptr<MenuItem>>* MenuNode::getChildNodes() {
  return &this->childNodes;
}

void MenuNode::addNode(std::shared_ptr<MenuItem> node) {
  this->childNodes.push_back(node);
}

void MenuNode::up() {
  moveSelection(-1);
}

void MenuNode::down() {
  moveSelection(1);
}

void MenuNode::pageUp(int pageSize) {
  moveSelection(-pageSize, false);
}

void MenuNode::pageDown(int pageSize) {
  moveSelection(pageSize, false);
}

void MenuNode::moveSelection(int delta, bool allowWrap) {
  if (childNodes.empty() || !delta) {
    return;
  }

  size_t numNodes = childNodes.size();
  if (delta > 0) {
    if (allowWrap) {
      selected = (selected + delta) % numNodes;
    } else {
      selected = std::min(selected + delta, numNodes - 1);
    }
    return;
  }

  size_t offset = delta * -1;
  if (offset <= selected) {
    selected -= offset;
    return;
  }

  if (!allowWrap) {
    selected = 0;
  } else {
    offset -= selected;
    while (offset > numNodes) {
      offset -= numNodes;
    }
    selected = numNodes - offset;
  }
}

/******************************************************************************************
                               MenuNonInteractive
******************************************************************************************/
MenuNonInteractive::MenuNonInteractive(MenuNode* parent, std::string const& label) :
    MenuNode(parent, label) {
}

void MenuNonInteractive::execute(Menu*) {
  // Do nothing, this node cannot be entered.
}

/******************************************************************************************
                                   MenuXbe
******************************************************************************************/
MenuXbe::MenuXbe(MenuNode* parent, std::string const& label, std::string const& paths) :
    MenuNode(parent, label) {

  size_t path_start = 0;
  for (size_t path_end = paths.find(PATH_DELIMITER, path_start);
       path_end != std::string::npos; path_end = paths.find(PATH_DELIMITER, path_start)) {

    remainingScanPaths.emplace_back(paths.substr(path_start, path_end - path_start));
    path_start = path_end + 1;
  }
  remainingScanPaths.emplace_back(paths.substr(path_start));

  if (!remainingScanPaths.empty()) {
    updateScanningLabel();
    XBEScanner::scanPath(
        remainingScanPaths.front(),
        [this](bool succeeded, std::list<XBEInfo> const& items, long long duration) {
          this->onScanCompleted(succeeded, items, duration);
        });
  }
}

MenuXbe::~MenuXbe() {
}

void MenuXbe::execute(Menu* menu) {
  if (menu->getCurrentMenu() != this) {
    menu->setCurrentMenu(this);
  } else {
    if (childNodes.size() > selected) {
      this->childNodes.at(selected)->execute(menu);
    }
  }
}

std::vector<std::shared_ptr<MenuItem>>* MenuXbe::getChildNodes() {
  std::lock_guard<std::mutex> lock(childNodesLock);
  return MenuNode::getChildNodes();
}

void MenuXbe::superscroll(bool moveToPrevious) {
  std::lock_guard<std::mutex> lock(childNodesLock);

  if (childNodes.empty() || superscrollIndex.empty()) {
    return;
  }

  const auto& currentItemLabel = childNodes[selected]->getLabel();
  int firstChar = tolower(currentItemLabel[0]);

  auto it = superscrollIndex.find(firstChar);
  if (it == superscrollIndex.end()) {
    InfoLog::outputLine("Failed to find %d (%c) in superscroll index", firstChar,
                        static_cast<char>(firstChar));
    return;
  }

  if (moveToPrevious) {
    if (it == superscrollIndex.begin()) {
      return;
    }

    --it;
  } else {
    ++it;
    if (it == superscrollIndex.end()) {
      return;
    }
  }

  selected = it->second;
}

void MenuXbe::updateScanningLabel() {
  std::string scanningLabel = "Scanning \"" + remainingScanPaths.front() + "\" ...";
  std::lock_guard<std::mutex> lock(childNodesLock);
  childNodes.clear();
  childNodes.push_back(std::make_shared<MenuNonInteractive>(this, scanningLabel));
}

void MenuXbe::onScanCompleted(bool succeeded,
                              std::list<XBEInfo> const& items,
                              long long duration) {
  (void)duration;
  std::string path = remainingScanPaths.front();
  remainingScanPaths.pop_front();

  if (!succeeded) {
    InfoLog::outputLine("Failed to scan '%s' for XBEs, skipping...\n", path.c_str());
  } else {
    discoveredItems.insert(discoveredItems.end(), std::make_move_iterator(begin(items)),
                           std::make_move_iterator(end(items)));
  }

  if (!remainingScanPaths.empty()) {
    updateScanningLabel();
    XBEScanner::scanPath(
        remainingScanPaths.front(),
        [this](bool succeeded, std::list<XBEInfo> const& items, long long duration) {
          this->onScanCompleted(succeeded, items, duration);
        });
    return;
  }

  createChildren();
}

void MenuXbe::createChildren() {
  std::vector<std::shared_ptr<MenuItem>> newChildren;

  for (auto& info: discoveredItems) {
    XPR0Image saveIcon;
    info.loadCompressedSaveGameIcon(saveIcon);
    newChildren.push_back(std::make_shared<MenuLaunch>(info.title, info.path, saveIcon));
  }

  std::sort(begin(newChildren), end(newChildren),
            [](const std::shared_ptr<MenuItem>& a, const std::shared_ptr<MenuItem>& b) {
              return SI::natural::compare(a->getLabel(), b->getLabel());
            });

  std::map<int, size_t> index;
  int lastProcessed = -1;
  for (size_t i = 0; i < newChildren.size(); ++i) {
    const auto& childLabel = newChildren[i]->getLabel();

    if (childLabel.empty()) {
      InfoLog::outputLine("Child with empty label while building superscroll index.");
      continue;
    }

    int firstChar = tolower(childLabel[0]);
    if (firstChar == lastProcessed) {
      continue;
    }

    index[firstChar] = i;
    lastProcessed = firstChar;
  }

  std::lock_guard<std::mutex> lock(childNodesLock);
  childNodes = newChildren;
  superscrollIndex = index;
}


/******************************************************************************************
                                   MenuLaunch
******************************************************************************************/
MenuLaunch::MenuLaunch(std::string const& label, std::string path) :
    MenuItem(label), path(std::move(path)), image() {
}

MenuLaunch::MenuLaunch(std::string const& label, std::string path, XPR0Image image) :
    MenuItem(label), path(std::move(path)), image(std::move(image)) {
}

MenuLaunch::~MenuLaunch() {
}

void MenuLaunch::execute(Menu*) {
  InfoLog::outputLine("Launching xbe %s\n", this->path.c_str());
#ifdef NXDK
  XLaunchXBE(const_cast<char*>(this->path.c_str()));
#endif
}

/******************************************************************************************
                                   MenuExec
******************************************************************************************/
MenuExec::MenuExec(std::string const& label, void execute(Menu*)) :
    MenuItem(label), action(execute) {
}

MenuExec::~MenuExec() {
}

void MenuExec::execute(Menu* menu) {
  action(menu);
}
/******************************************************************************************
                                   Menu
******************************************************************************************/
Menu::Menu(const Config& config, Renderer& renderer) :
    renderer(renderer), rootNode("Main menu") {
  rootNode.setParent(&rootNode);
  currentMenu = &rootNode;
  menuHeight = renderer.getHeight() * 0.9;
  startHeight = renderer.getHeight() * 0.1;
  for (nlohmann::json const& e: config.menu) {
    if (!static_cast<std::string>(e["type"]).compare("scan")) {
      std::shared_ptr<MenuXbe> newNode = std::make_shared<MenuXbe>(currentMenu, e["label"],
                                                                   e["path"]);
      this->rootNode.addNode(newNode);
    } else if (!static_cast<std::string>(e["type"]).compare("launch")) {
      std::shared_ptr<MenuLaunch> newNode = std::make_shared<MenuLaunch>(e["label"],
                                                                         e["path"]);
      this->rootNode.addNode(newNode);
    } else if (!static_cast<std::string>(e["type"]).compare("reboot")) {
      std::shared_ptr<MenuExec> newNode = std::make_shared<MenuExec>(
          e["label"], [](Menu*) { exit(0); });
      this->rootNode.addNode(newNode);
    } else if (!static_cast<std::string>(e["type"]).compare("settings")) {
      std::shared_ptr<MenuNode> newNode = std::make_shared<settingsMenu>(currentMenu,
                                                                         e["label"]);
      this->rootNode.addNode(newNode);
    }
  }

  // TODO: Take the repeat interval from the config.
  autoRepeatIntervals[SDL_CONTROLLER_BUTTON_DPAD_UP] = 250;
  autoRepeatIntervals[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = 250;
}

void Menu::render(Font& font) {
  std::pair<float, float> coordinates(100, startHeight);
  std::string menutext = std::string(this->currentMenu->getLabel());
  font.draw(menutext, std::make_pair<int, int>(300, 20));

  if (this->currentMenu->getChildNodes()->empty()) {
    font.draw("<Empty>", coordinates);
    return;
  }

  size_t selected = static_cast<int>(this->currentMenu->getSelected());
  if (this->currentMenu->getChildNodes()->size() <= this->itemsToShow) {
    offsetForDraw = 0;
  } else {
    if (selected > (this->currentMenu->getChildNodes()->size() - lowerHalf)) {
      offsetForDraw = (this->currentMenu->getChildNodes()->size() - itemsToShow);
    } else if (selected < upperHalf) {
      offsetForDraw = 0;
    } else {
      offsetForDraw = selected - upperHalf;
    }
  }

  size_t i = offsetForDraw;
  for (auto it = begin(*this->currentMenu->getChildNodes()) + offsetForDraw;
       it != end(*this->currentMenu->getChildNodes()); ++it) {
    menutext = std::string((*it)->getLabel());
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

    coordinates = std::pair<float, float>(
        std::get<0>(coordinates), std::get<1>(coordinates) + std::get<1>(dimensions));
    if (std::get<1>(coordinates) > menuHeight) {
      break;
    }
    ++i;
  }
}

MenuNode* Menu::getCurrentMenu() {
  return currentMenu;
}

void Menu::setCurrentMenu(MenuNode* menu) {
  currentMenu = menu;
}

void Menu::onUpPressed() {
  currentMenu->up();
}

void Menu::onDownPressed() {
  currentMenu->down();
}

void Menu::pageUp() {
  currentMenu->pageUp(static_cast<int>(itemsToShow));
}

void Menu::pageDown() {
  currentMenu->pageDown(static_cast<int>(itemsToShow));
}

void Menu::superscrollUp() {
  currentMenu->superscrollUp();
}

void Menu::superscrollDown() {
  currentMenu->superscrollDown();
}

void Menu::back() {
  InfoLog::outputLine("Setting menu to %s\n",
                      std::string(currentMenu->getParent()->getLabel()).c_str());
  currentMenu = currentMenu->getParent();
}

void Menu::execute() {
  this->currentMenu->execute(this);
}
