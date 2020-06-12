#ifndef __MENU_H
#define __MENU_H
#include <vector>
#include <NevolutionX/menu/menuItem.h>
#include <NevolutionX/render/renderer.h>
#include <NevolutionX/render/font.h>
#endif
class menu {
  Renderer *r;
  Font *f;
  std::vector<xbeMenuItem> *gamesList;
  std::vector<xbeMenuItem> *appsList;
  std::vector<menuItem> mainMenu;
  int mainMenuSelection = 0;
  size_t currItem = 0, prevItem = 0, listSize = 0;
public:
  menu(Renderer *rp, Font *fp, std::vector<xbeMenuItem> *gL, std::vector<xbeMenuItem> *aL);
  void goToMainMenu(menuItem *mI);
  void up();
  void down();
  void back();
  void A();
  int render();
};
