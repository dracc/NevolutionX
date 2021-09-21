#include "audioMenu.hpp"
#include <xboxkrnl/xboxkrnl.h>
#include "eeprom.hpp"

audioChannelsMenuItem::audioChannelsMenuItem() :
    switchingMenuItem("Channels", { "Stereo", "Mono", "Surround" }) {
  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex);
  selected = Value & 0x00000003;
  valuedLabel = "Channels: " + values.at(selected);
}

void audioChannelsMenuItem::execute(Menu*) {
  selected = (++selected) % values.size();
  valuedLabel = label + values.at(selected);

  ULONG Type = 0;
  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex, Type);
  Value = (Value & 0xFFFFFFF0) + selected;
  ExSaveNonVolatileSetting(valueIndex, Type, &Value, sizeof(Value));
}

audioMenu::audioMenu(MenuNode* parent, std::string const& label) : MenuNode(parent, label) {
  addNode(std::make_shared<audioChannelsMenuItem>());
  addNode(
      std::make_shared<togglingEEPROMMenuItem>("Dolby Digital enabled: ", 0x9, 0x00010000));
  addNode(std::make_shared<togglingEEPROMMenuItem>("DTS enabled: ", 0x9, 0x00020000));
}
