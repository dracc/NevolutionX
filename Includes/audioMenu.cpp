#include "audioMenu.hpp"
#include "eeprom.hpp"

#ifdef NXDK
#include <xboxkrnl/xboxkrnl.h>
#endif
audioChannelsMenuItem::audioChannelsMenuItem() :
    switchingMenuItem("Channels", { "Stereo", "Mono", "Surround" }) {
#ifdef NXDK
  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex);
  selected = Value & 0x00000003;
  valuedLabel = "Channels: " + values.at(selected);
#endif
}

void audioChannelsMenuItem::execute(Menu*) {
#ifdef NXDK
  selected = (++selected) % values.size();
  valuedLabel = label + values.at(selected);

  ULONG Type = 0;
  uint32_t Value = getEEPROMValue<uint32_t>(valueIndex, Type);
  Value = (Value & 0xFFFFFFF0) + selected;
  ExSaveNonVolatileSetting(valueIndex, Type, &Value, sizeof(Value));
#endif
}

audioMenu::audioMenu(MenuNode* parent, std::string const& label) : MenuNode(parent, label) {
  addNode(std::make_shared<audioChannelsMenuItem>());
  addNode(
      std::make_shared<togglingEEPROMMenuItem>("Dolby Digital enabled: ", 0x9, 0x00010000));
  addNode(std::make_shared<togglingEEPROMMenuItem>("DTS enabled: ", 0x9, 0x00020000));
}
