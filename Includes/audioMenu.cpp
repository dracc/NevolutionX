#include "audioMenu.h"

#include <utility>
#include <vector>
#include <sstream>

static 
std::vector<std::pair<std::string, EEasyXB::AudioMode>> audioModes{
  {"Mono", EEasyXB::AudioMode::MONO},
  {"Stereo", EEasyXB::AudioMode::STEREO},
  {"Surround", EEasyXB::AudioMode::SURROUND},
  {"AC3", EEasyXB::AudioMode::AC3},
  {"DTS", EEasyXB::AudioMode::DTS}};

AudioItem::AudioItem(MenuNode *parent, std::string const& label, EEasyXB::AudioMode flag) :
  MenuItem(parent, label) {
  bitFlag = flag;
}

const char* DIVIDER = " : ";
const char* ENABLED = "Enabled";
const char* DISABLED = "Disabled";

void AudioItem::execute(Menu *menu) {
//#ifdef NXDK
  updateEnabledState(true);
  
  std::vector<std::shared_ptr<MenuItem>> *menuItems = parentNode->getChildNodes();
  for (auto iter = (*menuItems).begin(); iter != (*menuItems).end(); ++iter) {
    std::shared_ptr<AudioItem> thisAudioItem = std::static_pointer_cast<AudioItem>(*iter);
    EEasyXB::AudioMode flag = thisAudioItem->getBitFlag();

    if(flag != bitFlag) {
      bool isEnabled = EEasyXB::Eeprom::GetInstance()->IsAudioModeEnabled(flag);
      thisAudioItem->setEnabledLabel(isEnabled);
    }
  }
//#endif
}

void AudioItem::updateEnabledState(bool toggleEnabledState) {
  EEasyXB::Eeprom* eeprom = EEasyXB::Eeprom::GetInstance();

  bool activeState = eeprom->IsAudioModeEnabled(bitFlag);

  if(toggleEnabledState) {
    eeprom->SetAudioModeEnabled(bitFlag, !activeState);
    activeState = eeprom->IsAudioModeEnabled(bitFlag);
  }

  setEnabledLabel(activeState);
  EEasyXB::Eeprom::GetInstance()->Write();
}

void AudioItem::setEnabledLabel(bool isEnabled) {
  std::ostringstream oss;
  oss << DIVIDER << (isEnabled ? ENABLED : DISABLED);
  setDynamicText(oss.str());
}

AudioMenu::AudioMenu(MenuNode *parent, std::string const& label) :
  MenuNode(parent, label) {
  for (auto &e: audioModes) {
    EEasyXB::Eeprom::GetInstance()->Read();
    bool isItemEnabled = EEasyXB::Eeprom::GetInstance()->IsAudioModeEnabled(e.second);
    std::shared_ptr<AudioItem> newNode = std::make_shared<AudioItem>(this, e.first, e.second);
    newNode->setEnabledLabel(isItemEnabled);
    addNode(newNode);
  }
}