#ifdef NXDK
#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#endif

#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stdbool.h"
#include <SDL.h>
#include "SDL_ttf.h"
#include <threads.h>

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

void outputLine(char* format, ...) {
  char buffer[4096];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
#ifdef NXDK
  debugPrint("%s", buffer);
#else
  SDL_Log("%s", buffer);
#endif
  va_end(args);
}

typedef struct menuItem{
  SDL_Texture* texture;
  bool selected;
  int height, width;
} menuItem;

void drawMenuItems(SDL_Renderer *renderer, menuItem const items[]) {
  SDL_Rect viewport;
  SDL_Rect position;
  int i;

  SDL_RenderGetViewport(renderer, &viewport);
  SDL_RenderClear(renderer);
  for (i = 0; i < 5; ++i) {
    SDL_RenderCopy(renderer, items[i].texture, NULL, &position);
    position.y += items[i].height;
  }
  SDL_RenderPresent(renderer);
}

int muhLoop() {
  int i, done;
#ifndef NXDK
  int SCREEN_WIDTH = 320, SCREEN_HEIGHT = 240;
#endif
  const char *driver;
  Uint32 windowFlags;
  SDL_Window *window;
  SDL_Renderer *renderer;
  const char *items[] = {"Games", "Movies", "Music", "Pictures", "Settings"};
  int window_width, window_height;
  SDL_Event event;
  TTF_Font *font;
  SDL_Color color = {0xFF, 0x7F, 0xFF, 0xFF};
  menuItem menuItems[5];
  
  if (SDL_VideoInit(NULL) != 0) {
    outputLine("Init error: %s", SDL_GetError());
#ifdef NXDK
    XSleep(2000);
    XReboot();
#endif
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
                              SDL_WINDOW_SHOWN, &window, &renderer);
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

  for (i = 0; i < 5; ++i) {
    SDL_Surface *tmp = TTF_RenderUTF8_Blended(font, items[i], color);
    if (tmp == NULL) {
      outputLine("TTF Error: %s", TTF_GetError());
      thrd_exit(4);
    }
    menuItems[i].texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_QueryTexture(menuItems[i].texture, NULL, NULL, &menuItems[i].width,
                     &menuItems[i].height);
  }

  done = 0;
  while (!done) {
#ifdef NXDK
#else
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
      default:
        break;
      }
    }
#endif
    drawMenuItems(renderer, menuItems);
  }
  
  for (i = 0; i < 30; ++i) {
    outputLine("%d\n", i);
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
  XReboot();
#endif
  SDL_Quit();
  return 0;
}


#ifdef NXDK
#else
#endif
