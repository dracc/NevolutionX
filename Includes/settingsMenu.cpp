#include "settingsMenu.hpp"
#include "langMenu.hpp"
#include "timeMenu.hpp"

settingsMenu::settingsMenu(MenuNode *parent, std::string const& label) :
MenuNode(parent, label) {
  init();
}

void settingsMenu::init() {
  addNode(std::make_shared<LangMenu>(this, "Language select"));
  addNode(std::make_shared<TimeMenu>(this, "Timezone select"));
}

