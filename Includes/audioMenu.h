#ifndef AUDIOMENU_H
#define AUDIOMENU_H

#include "menu.hpp"
#include "Eeprom.h"
#include <string>

class AudioItem : public MenuItem {
public:
    AudioItem(MenuNode *parent, std::string const& label, EEasyXB::AudioMode flag);
    void execute(Menu *menu);
    void setEnabledLabel(bool isEnabled);
    EEasyXB::AudioMode getBitFlag()   {return bitFlag;}
private:
    EEasyXB::AudioMode bitFlag;
    void updateEnabledState(bool toggleEnabledState);
};

class AudioMenu : public MenuNode {
public:
    AudioMenu(MenuNode *parent, std::string const& label);
private:
};

#endif