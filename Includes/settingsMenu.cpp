#include "settingsMenu.hpp"
#include "audioMenu.hpp"
#include "eeprom.hpp"
#include "langMenu.hpp"
#include "logViewerMenu.h"
#include "timeMenu.hpp"
#include "videoMenu.hpp"
#include "wipeCache.hpp"

#ifdef NXDK
#include <xboxkrnl/xboxkrnl.h>
#endif

/******************************************************************************************
 *                              switchingMenuItem
 *****************************************************************************************/
switchingMenuItem::switchingMenuItem(std::string const& label, std::vector<std::string> s) :
    MenuItem(label), values(s) {
}

std::string_view switchingMenuItem::getLabel() const {
  return valuedLabel;
}

/******************************************************************************************
 *                              togglingEEPROMMenuItem
 *****************************************************************************************/
togglingEEPROMMenuItem::togglingEEPROMMenuItem(std::string const& label,
                                               uint32_t vI,
                                               uint32_t bmask) :
    MenuItem(label),
    bitmask(bmask), valueIndex(vI) {

#ifdef NXDK
  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex);
  value = (Value & bitmask) != 0;

  valuedLabel = label + (value ? "Yes" : "No");
#endif
}

void togglingEEPROMMenuItem::execute(Menu*) {
#ifdef NXDK
  value = !value;
  valuedLabel = label + (value ? "Yes" : "No");

  ULONG Type = 0;
  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex, Type);
  Value = (Value & ~bitmask) + (bitmask * value);
  ExSaveNonVolatileSetting(valueIndex, Type, &Value, sizeof(Value));
#endif
}

std::string_view togglingEEPROMMenuItem::getLabel() const {
  return valuedLabel;
}

/******************************************************************************************
 *                               settingsMenu
 *****************************************************************************************/
settingsMenu::settingsMenu(MenuNode* parent, std::string const& label) :
    MenuNode(parent, label) {
  init();
}

void settingsMenu::init() {
  addNode(std::make_shared<videoMenu>(this, "Video"));
  addNode(std::make_shared<audioMenu>(this, "Audio"));
  addNode(std::make_shared<LangMenu>(this, "Language select"));
  addNode(std::make_shared<TimeMenu>(this, "Timezone select"));
  addNode(std::make_shared<logViewerMenu>(this, "Log viewer"));
  addNode(std::make_shared<MenuExec>("Wipe cache partitions", [](Menu*) { wipe_cache(); }));
  ;
}
