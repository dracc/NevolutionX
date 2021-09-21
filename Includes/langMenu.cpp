#include "langMenu.hpp"

#ifdef NXDK
#include <xboxkrnl/xboxkrnl.h>
#endif

static std::vector<std::pair<std::string, uint32_t>> languages{
  { "English", 1 }, { "Japanese", 2 }, { "German", 3 },
  { "French", 4 },  { "Spanish", 5 },  { "Italian", 6 },
  { "Korean", 7 },  { "Chinese", 8 },  { "Portuguese", 9 }
};

LangItem::LangItem(MenuNode* parent, std::string const& label, uint32_t langnum) :
    MenuItem(parent, label), langnum(langnum) {
}

void LangItem::execute(Menu* menu) {
#ifdef NXDK
  ExSaveNonVolatileSetting(0x7, 4, &langnum, sizeof(langnum));
#endif
  menu->setCurrentMenu(this->parentNode->getParent());
}

LangMenu::LangMenu(MenuNode* parent, std::string const& label) : MenuNode(parent, label) {
  for (auto& e: languages) {
    std::shared_ptr<LangItem> newNode = std::make_shared<LangItem>(this, e.first, e.second);
    addNode(newNode);
  }
}
