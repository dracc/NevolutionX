#include "menuItemList.h"

menuItemList mil_createNew() {
  menuItemList ret = {NULL, NULL, 0, NULL};
  return ret;
}

void mil_setFont(menuItemList *mil, TTF_Font *font) {
  mil->font = font;
}

void mil_push(menuItemList *mil, menuItem *item) {
  item->next = mil->head;
  if (mil->head == NULL) {
    mil->tail = item;
  }
  mil->head = item;
  ++mil->length;
}

void mil_push_c(menuItemList *mil, char *item) {
  menuItem *tmp = (menuItem*)malloc(sizeof(menuItem));
  memcpy(tmp, &(menuItem){item, NULL, NULL, 0, 0, NULL}, sizeof(menuItem));
  mil_push(mil, tmp);
}

void mil_append(menuItemList *mil, menuItem *item) {
  item->next = NULL;
  if (mil->head == NULL) {
    mil->head = item;
  } else {
    mil->tail->next = item;
  }
  mil->tail = item;
  ++mil->length;
}

void mil_append_c(menuItemList *mil, char *item) {
  menuItem *tmp = (menuItem*)malloc(sizeof(menuItem));
  memcpy(tmp, &(menuItem){item, NULL, NULL, 0, 0, NULL}, sizeof(menuItem));
  mil_append(mil, tmp);
}

void mil_append_list(menuItemList *mil, ...) {
  va_list args;
  va_start(args, mil);
  char *tmp;
  while ((tmp = va_arg(args, char*)) != NULL) {
    mil_append_c(mil, tmp);
  }
  va_end(args);
}

menuItem *mil_pop(menuItemList *mil) {
  if (mil->head == NULL) {
    return NULL;
  }
  menuItem *ret = mil->head;
  mil->head = ret->next;
  --mil->length;
  return ret;
}

bool mil_isEmpty(menuItemList *mil) {
  return mil->head == NULL;
}

int mil_length(menuItemList *mil) {
  return mil->length;
}

menuItem *mil_find(menuItemList *mil, int key) {
  if (mil->length <= key) {
    return NULL;
  }
  menuItem *ret = mil->head;
  while (key) {
    --key;
    ret = ret->next;
  }
  return ret;
}

menuItem *mil_remove(menuItemList *mil, int key) {
  if (mil->length <= key) {
    return NULL;
  }
  menuItem *tmp = mil->head;
  menuItem *ret = NULL;
  while (key) {
    --key;
    tmp = tmp->next;
  }
  ret = tmp->next;
  tmp->next = tmp->next->next;
  return ret;
}

void helper_genTexture(SDL_Renderer *renderer, menuItem *itm, TTF_Font *font,
                       SDL_Color col) {
  if (itm->texture != NULL) {
    SDL_DestroyTexture(itm->texture);
  }
  SDL_Surface *surf = TTF_RenderText_Blended(font, itm->label, col);
  if (surf == NULL) {
    outputLine("TTF_Error: %s", TTF_GetError());
    thrd_exit(4);
  }
  itm->texture = SDL_CreateTextureFromSurface(renderer, surf);
  if (itm->texture == NULL) {
    outputLine("Texture creation failed: %s", SDL_GetError());
    thrd_exit(5);
  }
  SDL_FreeSurface(surf);
  SDL_QueryTexture(itm->texture, NULL, NULL, &(itm->width), &(itm->height));
}

void mil_generateTextures(SDL_Renderer *renderer, menuItemList *mil,
                          int selected) {
  menuItem *tmp = mil->head;
  int i = 0;
  SDL_Color colorSelected = {0xFF, 0x7F, 0xFF, 0xFF};
  SDL_Color colorPassive = {0x7F, 0xFF, 0xFF, 0xFF};
  SDL_Color color;
  while (tmp != NULL) {
    if (i != selected) {
      color = colorPassive;
    } else {
      color = colorSelected;
    }
    helper_genTexture(renderer, tmp, mil->font, color);
    ++i;
    tmp = tmp->next;
  }
}

void mil_updateTexture(SDL_Renderer *renderer, menuItemList *mil, int key) {
  menuItem *tmp = mil_find(mil, key);
  SDL_Color color = {0x7F, 0xFF, 0xFF, 0xFF};
  helper_genTexture(renderer, tmp, mil->font, color);
}

void mil_updatePrevCurrAndNextTexture(SDL_Renderer *renderer, menuItemList *mil,
                                      int key) {
  if (key == 0) {
    key = mil->length - 1;
  } else {
    --key;
  }
  menuItem *tmp = mil_find(mil, key);
  SDL_Color colorSelected = {0xFF, 0x7F, 0xFF, 0xFF};
  SDL_Color colorPassive = {0x7F, 0xFF, 0xFF, 0xFF};
  helper_genTexture(renderer, tmp, mil->font, colorPassive);
  if (tmp == mil->tail) {
    tmp = mil->head;
  } else {
    tmp = tmp->next;
  }
  helper_genTexture(renderer, tmp, mil->font, colorSelected);
  if (tmp == mil->tail) {
    tmp = mil->head;
  } else {
    tmp = tmp->next;
  }
  helper_genTexture(renderer, tmp, mil->font, colorPassive);
}

void mil_drawMenuItems(SDL_Renderer *renderer, menuItemList *mil) {
  SDL_Rect position = {20, 20, 0, 0};
  int renderResult;

  menuItem *tmp = mil->head;
  
  SDL_RenderClear(renderer);
  while (tmp != NULL) {
    position.h = tmp->height;
    position.w = tmp->width;
    renderResult = SDL_RenderCopy(renderer, tmp->texture, NULL, &position);
    if (renderResult != 0) {
      outputLine("menuItem RenderCopy failed: %s", SDL_GetError());
      thrd_exit(3);
    }
    position.y += tmp->height;
    tmp = tmp->next;
  }
}

void mil_empty(menuItemList *mil) {
  menuItem *tmp = NULL;
  while (mil->head != NULL) {
    tmp = mil_pop(mil);
    mi_destroy(tmp);
    free(tmp);
  }
}

void mil_free(menuItemList *mil) {
  mil_empty(mil);
  TTF_CloseFont(mil->font);
  mil->font = NULL;
}

void mi_destroy(menuItem *mi) {
  SDL_DestroyTexture(mi->texture);
  mi->texture = NULL;
  mi->label = NULL;
  mi->next = NULL;
}
