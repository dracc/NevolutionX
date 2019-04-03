#ifdef NXDK
#ifndef XPADINPUT_H
#define XPADINPUT_H

#include "hal/input.h"

#include <stdbool.h>

_Bool getAnalogKeyDown (XPAD_INPUT *pad, int requested_key);

_Bool getDigitalKeyDown (XPAD_INPUT *pad, int requested_key);

#endif //XPADINPUT_H
#endif //NXDK
