#ifndef MENUITEMLIST_H
#define MENUITEMLIST_H

#include "outputLine.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <threads.h>

typedef struct menuItem{
  char *label;
  char *itemPath;
  SDL_Texture *texture;
  int height, width;
  struct menuItem *next;
} menuItem;

typedef struct menuItemList{
  menuItem *head;
  menuItem *tail;
  int length;
  TTF_Font *font;
} menuItemList;

menuItemList mil_createNew();

void mil_setFont(menuItemList *mil, TTF_Font *font);

void mil_push(menuItemList *mil, menuItem *item);
void mil_push_c(menuItemList *mil, char *item);

void mil_append(menuItemList *mil, menuItem *item);
void mil_append_c(menuItemList *mil, char *item);
void mil_append_list(menuItemList *mil, ...);

menuItem *mil_pop(menuItemList *mil);

bool mil_isEmpty(menuItemList *mil);

int mil_length(menuItemList *mil);

menuItem *mil_find(menuItemList *mil, int key);
menuItem *mil_remove(menuItemList *mil, int key);

void mil_generateTextures(SDL_Renderer *renderer, menuItemList *mil, int selected);
void mil_updateTexture(SDL_Renderer *renderer, menuItemList *mil, int key);
void mil_updatePrevCurrAndNextTexture(SDL_Renderer *renderer, menuItemList *mil, int key);

void mil_drawMenuItems(SDL_Renderer *renderer, menuItemList *mil);

void mil_empty(menuItemList *mil);
void mil_free(menuItemList *mil);

void mi_destroy(menuItem *mi);

#endif
