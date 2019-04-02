#include "xpadinput.h"

_Bool getKeyDown (XPAD_INPUT *pad, int requested_key) {
  return pad->PressedButtons.ucAnalogButtons[requested_key];
}
