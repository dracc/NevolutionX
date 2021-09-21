#include "videoMenu.hpp"
#include <xboxkrnl/xboxkrnl.h>
#include "eeprom.hpp"

static const uint32_t widescreen = 0x00010000;
static const uint32_t letterbox = 0x00100000;

static const std::vector<uint32_t> ratios = { 0, widescreen, letterbox };

videoRatioMenuItem::videoRatioMenuItem() :
    switchingMenuItem("Screen Ratio: ", { "Normal", "Widescreen", "Letterbox" }) {

  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex);
  selected = Value & 0x00110000;
  if (selected & widescreen) {
    selected = 1;
  } else if (selected & letterbox) {
    selected = 2;
  }
  valuedLabel = label + values.at(selected);
}

void videoRatioMenuItem::execute(Menu*) {
  selected = (++selected) % values.size();
  valuedLabel = label + values.at(selected);

  ULONG Type = 0;
  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex, Type);
  Value = (Value & 0xFFEEFFFF) + ratios.at(selected);
  ExSaveNonVolatileSetting(valueIndex, Type, &Value, sizeof(Value));
}

videoMenu::videoMenu(MenuNode* parent, std::string const& label) : MenuNode(parent, label) {
  addNode(std::make_shared<videoRatioMenuItem>());
  addNode(std::make_shared<togglingEEPROMMenuItem>("480p: ", 0x8, 0x00080000));
  addNode(std::make_shared<togglingEEPROMMenuItem>("720p: ", 0x8, 0x00020000));
  addNode(std::make_shared<togglingEEPROMMenuItem>("1080i: ", 0x8, 0x00040000));
}
