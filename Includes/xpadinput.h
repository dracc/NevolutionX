#ifdef NXDK
#ifndef XPADINPUT_H
#define XPADINPUT_H

#include "hal/input.h"

bool getAnalogKeyDown (XPAD_INPUT *pad, int requested_key);
bool getDigitalKeyDown (XPAD_INPUT *pad, int requested_key);

#endif //XPADINPUT_H
#endif //NXDK
