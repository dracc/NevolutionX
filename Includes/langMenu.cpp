#include "langMenu.hpp"
#include <algorithm>

#ifdef NXDK
#include <xboxkrnl/xboxkrnl.h>
#endif

static
std::vector<std::pair<std::string, uint32_t>> languages{
  {"Unknown", 0},
  {"English", 1},
  {"Japanese", 2},
  {"German", 3},
  {"French", 4},
  {"Spanish", 5},
  {"Italian", 6},
  {"Korean", 7},
  {"Chinese", 8},
  {"Portuguese", 9}};

LangItem::LangItem(MenuNode *parent, std::string const& label, uint32_t langnum) :
  MenuItem(parent, label), langnum(langnum) {

}

void LangItem::execute(Menu *menu) {
  if (langnum == 0) {
    return;
  }
#ifdef NXDK
  ExSaveNonVolatileSetting(0x7, 4, &langnum, sizeof(langnum));
#endif
  menu->setCurrentMenu(this->parentNode->getParent());
}

LangMenu::LangMenu(MenuNode *parent, std::string const& label) :
  MenuNode(parent, label) {
  for (auto &e: languages) {
    std::shared_ptr<LangItem> newNode = std::make_shared<LangItem>(this, e.first, e.second);
    addNode(newNode);
  }

#ifdef NXDK
  ULONG type = 0;
  ULONG resultLength = 0;
  ExQueryNonVolatileSetting(0x7, &type, &currLang, sizeof(currLang), &resultLength);
  setSelected(std::max((int)currLang - 1, 0));
  setLangLabel(std::get<0>(languages[currLang]));
#endif
}


std::string_view LangMenu::getLabel() const {
  return langLabel;
}


void LangMenu::execute(Menu *menu) {
  if (menu->getCurrentMenu() != this) {
    menu->setCurrentMenu(this);
  }
  else {
    if (childNodes.size() == 0 || selected == 0) {
      return;
    }
    if (childNodes.size() > selected) {
      this->childNodes.at(selected)->execute(menu);
      currLang = selected;
      setLangLabel(std::get<0>(languages[selected]));
    }
  }
}

void LangMenu::setLangLabel(std::string const& lang) {
  langLabel = label + " (" + lang + ")";
}
