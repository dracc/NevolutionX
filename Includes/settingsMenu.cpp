#include "settingsMenu.hpp"
#include "langMenu.hpp"
#include "timeMenu.hpp"
#include "wipeCache.hpp"
#include "audioMenu.h"

settingsMenu::settingsMenu(MenuNode *parent, std::string const& label) :
MenuNode(parent, label) {
  init();
}

void settingsMenu::init() {
  addNode(std::make_shared<LangMenu>(this, "Language select"));
  addNode(std::make_shared<TimeMenu>(this, "Timezone select"));
  addNode(std::make_shared<AudioMenu>(this, "Audio settings"));
  addNode(std::make_shared<MenuExec>("Wipe cache partitions", [](Menu *){
    wipe_cache();
  }));
}

