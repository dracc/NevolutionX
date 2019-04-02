#ifndef NAVIGATEMENU_H
#define NAVIGATEMENU_H

#ifdef NXDK
#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#include "xpadinput.h"
#endif

#include "outputLine.h"

#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stdbool.h"
#include <SDL.h>
#include "SDL_ttf.h"
#include <threads.h>

#define NUMITEMS 4

typedef struct menuItem{
  SDL_Texture* texture;
  int height, width;
} menuItem;

void drawMenuItems(SDL_Renderer *renderer, menuItem const items[]);

void updateMenuItemTextures(SDL_Renderer *renderer, menuItem menuItems[],
                     const char *items[], TTF_Font *font, int selected);
int  menuLoop();

void finishRendering(SDL_Renderer *renderer);

#endif
