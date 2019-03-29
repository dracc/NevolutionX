#include "navigateMenu.h"


void drawMenuItems(SDL_Renderer *renderer, menuItem const items[]) {
  SDL_Rect viewport;
  SDL_Rect position;
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
