#include "navigateMenu.h"

#ifdef NXDK
const extern int SCREEN_WIDTH;
const extern int SCREEN_HEIGHT;
#else
int  SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;
#endif

void drawMenuItems(SDL_Renderer *renderer, menuItem const items[]) {
  SDL_Rect viewport;
  SDL_Rect position = {20, 20, 0, 0};
  int i;

  SDL_RenderGetViewport(renderer, &viewport);
  SDL_RenderClear(renderer);
  for (i = 0; i < NUMITEMS; ++i) {
    position.h = items[i].height;
    position.w = items[i].width;
    SDL_RenderCopy(renderer, items[i].texture, NULL, &position);
    position.y += items[i].height;
  }
  SDL_RenderPresent(renderer);
  #ifdef NXDK
  XVideoWaitForVBlank();
  #endif
}

void renderMenuItems(SDL_Renderer *renderer, menuItem menuItems[],
                     const char *items[], TTF_Font *font, int selected) {

  int i;
  SDL_Color colorSelected = {0xFF, 0x7F, 0xFF, 0xFF};
  SDL_Color colorPassive = {0x7F, 0xFF, 0xFF, 0xFF};
  SDL_Color color;
  for (i = 0; i < NUMITEMS; ++i) {
    SDL_DestroyTexture(menuItems[i].texture);
    if (i == selected) {
      color = colorSelected;
    } else {
      color = colorPassive;
    }
    SDL_Surface *tmp = TTF_RenderText_Blended(font, items[i], color);
    if (tmp == NULL) {
      outputLine("TTF Error: %s", TTF_GetError());
      thrd_exit(4);
    }
    menuItems[i].texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_QueryTexture(menuItems[i].texture, NULL, NULL, &(menuItems[i].width),
                     &(menuItems[i].height));
  }
}


int menuLoop() {
  int i, done, currItem, oldItem;
  const char *driver;
  Uint32 windowFlags,renderFlags;
  SDL_Window *window;
  SDL_Renderer *renderer;
  const char *items[NUMITEMS] = {"Games", "Launch DVD", "Applications", "Settings"};
  int window_width, window_height;
  SDL_Event event;
  TTF_Font *font;
  menuItem menuItems[NUMITEMS];
  
#ifdef NXDK
  windowFlags = SDL_WINDOW_SHOWN;
  renderFlags = 0;
#else
  windowFlags = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE;
  renderFlags = SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED;
#endif

  window = SDL_CreateWindow("NevolutionX",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
  renderer = SDL_CreateRenderer(window, -1, renderFlags);
                    
  if (window == NULL || renderer == NULL) {
    outputLine("Error: %s", SDL_GetError());
    thrd_exit(1);
  }

  SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0xE0, 0xFF);
  SDL_RenderClear(renderer);

  font = TTF_OpenFont("DejaVuSansMono.ttf", 16);
  if (font == NULL) {
    outputLine("TTF OpenFont() Error: %s", TTF_GetError());
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
