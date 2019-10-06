#ifndef OUTPUTLINE_H
#define OUTPUTLINE_H

#ifdef NXDK
#include <hal/debug.h>
#include "stdio.h"
#else
#include <SDL.h>
#endif

#include "stdarg.h"

void outputLine(const char* format, ...);

#endif // OUTPUTLINE_H
