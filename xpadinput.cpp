#include "xpadinput.h"
#ifdef NXDK

bool getAnalogKeyDown (XPAD_INPUT *pad, int requested_key) {
  return pad->PressedButtons.ucAnalogButtons[requested_key];
}

bool getDigitalKeyDown (XPAD_INPUT *pad, int requested_key) {
  return pad->PressedButtons.usDigitalButtons & requested_key;
}

#endif
