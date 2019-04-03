#include "navigateMenu.h"

#ifdef NXDK
const extern int SCREEN_WIDTH;
const extern int SCREEN_HEIGHT;
#else
int  SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;
#endif

void drawMenuItems(SDL_Renderer *renderer, menuItem const items[],
                   int itemCount) {
  SDL_Rect position = {20, 20, 0, 0};
  int i, renderResult;

  SDL_RenderClear(renderer);
  for (i = 0; i < itemCount; ++i) {
    position.h = items[i].height;
    position.w = items[i].width;
    renderResult = SDL_RenderCopy(renderer, items[i].texture, NULL, &position);
    if (renderResult != 0) {
      outputLine("menuItem renderCopy failed: %s", SDL_GetError());
      thrd_exit(3);
    }
    position.y += items[i].height;
  }
}

void updateMenuItemTextures(SDL_Renderer *renderer, menuItem menuItems[],
                            const char *items[], TTF_Font *font, int selected,
                            int itemCount) {

  int i;
  SDL_Color colorSelected = {0xFF, 0x7F, 0xFF, 0xFF};
  SDL_Color colorPassive = {0x7F, 0xFF, 0xFF, 0xFF};
  SDL_Color color;
  for (i = 0; i < itemCount; ++i) {
    if(menuItems[i].texture != NULL) {
      SDL_DestroyTexture(menuItems[i].texture);
    }
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
    if (menuItems[i].texture == NULL) {
      outputLine("Texture creation failed: %s", SDL_GetError());
      thrd_exit(5);
    }
    SDL_FreeSurface(tmp);
    SDL_QueryTexture(menuItems[i].texture, NULL, NULL, &(menuItems[i].width),
                     &(menuItems[i].height));
  }
}


int menuLoop() {
  int i, done, currItem;
  int itemCount = NUMITEMS;
  const char *driver;
  Uint32 windowFlags,renderFlags;
  SDL_Window *window;
  SDL_Renderer *renderer;
  const char *items[NUMITEMS] = {"Games", "Launch DVD", "Applications", "Settings"};
  int window_width, window_height;
  SDL_Event event;
  TTF_Font *font;
  menuItem menuItems[NUMITEMS] = {{NULL, 0, 0}};
  
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
  if (window == NULL) {
    outputLine("Window creation error: %s", SDL_GetError());
    thrd_exit(1);
  }

  renderer = SDL_CreateRenderer(window, -1, renderFlags);
  if (renderer == NULL) {
    outputLine("Renderer creation error: %s", SDL_GetError());
    thrd_exit(1);
  }

  if (SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0xE0, 0xFF) != 0) {
    outputLine("SetRenderDrawColor error: %s", SDL_GetError());
    thrd_exit(2);
  }

  if (SDL_RenderClear(renderer) != 0) {
    outputLine("RenderClear error: %s", SDL_GetError());
    thrd_exit(3);
  }

  font = TTF_OpenFont("DejaVuSansMono.ttf", 16);
  if (font == NULL) {
    outputLine("TTF_OpenFont() Error: %s", TTF_GetError());
    thrd_exit(3);
  }

  SDL_GetWindowSize(window, &window_width, &window_height);

  currItem = 0;
  updateMenuItemTextures(renderer, menuItems, items, font, currItem, itemCount);

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
            currItem = itemCount - 1;
          } else {
            --currItem;
          }
          updateMenuItemTextures(renderer, menuItems, items, font, currItem, itemCount);
          break;
        case SDLK_DOWN:
          currItem = (currItem + 1) % itemCount;
          updateMenuItemTextures(renderer, menuItems, items, font, currItem, itemCount);
          break;
        }
        break;
#endif
      default:
        break;
      }
    }
#ifdef NXDK
    XInput_GetEvents();
    for (i = 0; i < XInputGetPadCount(); ++i) {
      if (getDigitalKeyDown(&g_Pads[i], XPAD_DPAD_UP)) {
        if (currItem == 0) {
          currItem = itemCount - 1;
        } else {
          --currItem;
        }
        updateMenuItemTextures(renderer, menuItems, items, font, currItem, itemCount);
      }
      if (getDigitalKeyDown(&g_Pads[i], XPAD_DPAD_DOWN)) {
        currItem = (currItem + 1) % itemCount;
        updateMenuItemTextures(renderer, menuItems, items, font, currItem, itemCount);
      }
    }
#endif
    drawMenuItems(renderer, menuItems, itemCount);
    finishRendering(renderer);
  }
  thrd_exit(0);
}

void finishRendering(SDL_Renderer *renderer) {
  SDL_RenderPresent(renderer);
#ifdef NXDK
  XVideoWaitForVBlank();
#endif
}
