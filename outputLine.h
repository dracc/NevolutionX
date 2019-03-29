#ifndef OUTPUTLINE_H
#define OUTPUTLINE_H

#ifdef NXDK
#include <xboxrt/debug.h>
#endif

#include "stdarg.h"
#include <SDL.h>

void outputLine(char* format, ...);

#endif // OUTPUTLINE_H
