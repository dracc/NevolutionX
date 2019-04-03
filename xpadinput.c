#include "xpadinput.h"

_Bool getAnalogKeyDown (XPAD_INPUT *pad, int requested_key) {
  return pad->PressedButtons.ucAnalogButtons[requested_key];
}

_Bool getDigitalKeyDown (XPAD_INPUT *pad, int requested_key) {
  return pad->PressedButtons.usDigitalButtons & requested_key;
}
