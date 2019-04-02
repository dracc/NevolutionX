#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H

#ifdef NXDK
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/xbox.h>
#endif

#include "outputLine.h"

#include <SDL.h>
#include "SDL_ttf.h"

int init_systems();
void shutdown_systems(int systems);

#endif
