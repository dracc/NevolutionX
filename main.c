#ifdef NXDK
#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#endif

#include "outputLine.h"
#include "navigateMenu.h"

#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stdbool.h"
#include <SDL.h>
#include "SDL_ttf.h"
#include <threads.h>

#define NUMITEMS 5

#ifdef NXDK
int _exit(int x)
{
    debugPrint("_exit called\n");
    debugPrint( " SDL_Error: %s\n", SDL_GetError() );
    XSleep(5000);
    XReboot();
    return 0;
}

const extern int SCREEN_WIDTH;
const extern int SCREEN_HEIGHT;
#endif

int muhLoop() {
  int i, done, currItem, oldItem;
#ifndef NXDK
  int SCREEN_WIDTH, SCREEN_HEIGHT;
#endif
  const char *driver;
  Uint32 windowFlags;
  SDL_Window *window;
  SDL_Renderer *renderer;
  const char *items[NUMITEMS] = {"Games", "Movies", "Music", "Pictures", "Settings"};
  int window_width, window_height;
  SDL_Event event;
  TTF_Font *font;
  menuItem menuItems[NUMITEMS];
  
  if (SDL_VideoInit(NULL) != 0) {
    outputLine("Init error: %s", SDL_GetError());
    thrd_exit(1);
  }

#ifdef NXDK
  windowFlags = SDL_WINDOW_SHOWN;
#else
  windowFlags = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE;
#endif

  driver = SDL_GetCurrentVideoDriver();
  outputLine("%s", driver);

  SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT,
                              windowFlags, &window, &renderer);
  if (window == NULL || renderer == NULL) {
    outputLine("Error: %s", SDL_GetError());
    thrd_exit(1);
  }

  if (TTF_Init() != 0) {
    outputLine("TTF Error: %s", TTF_GetError());
    thrd_exit(2);
  }

  SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0xE0, 0xFF);
  SDL_RenderClear(renderer);

  font = TTF_OpenFont("DejaVuSansMono.ttf", 16);
  if (font == NULL) {
    outputLine("TTF Error: %s", TTF_GetError());
    thrd_exit(3);
  }

  SDL_GetWindowSize(window, &window_width, &window_height);

  currItem = 0;
  renderMenuItems(renderer, menuItems, items, font, currItem);

  done = 0;
  while (!done) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
        case SDL_WINDOWEVENT_EXPOSED:
          SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0xE0, 0xFF);
          SDL_RenderClear(renderer);
          break;
        }
        break;
      case SDL_QUIT:
        done = 1;
        break;
#ifndef NXDK
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_UP:
          if (currItem == 0) {
            currItem = NUMITEMS - 1;
          } else {
            --currItem;
          }
          renderMenuItems(renderer, menuItems, items, font, currItem);
          break;
        case SDLK_DOWN:
          currItem = (currItem + 1) % NUMITEMS;
          renderMenuItems(renderer, menuItems, items, font, currItem);
          break;
        }
        break;
#endif
      default:
        break;
      }
    }
    drawMenuItems(renderer, menuItems);
  }
  thrd_exit(0);
}

int main(void) {
#ifdef NXDK
  if (pb_init() != 0) {
    XSleep(2000);
    XReboot();
  }
#endif
  
  thrd_t thr;
  thrd_create(&thr, muhLoop, NULL);

  thrd_join(thr, NULL);
  
#ifdef NXDK
  XSleep(2000);
  XReboot();
#endif
  SDL_Quit();
  return 0;
}


#ifdef NXDK
#else
#endif
