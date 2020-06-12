/* dummy file, replace with thrimbors code */
#include <NevolutionX/menu/menu.h>
#include <NevolutionX/menu/xbeMenuItem.h>
#include <NevolutionX/menu/menuItem.h>
#include <NevolutionX/render/font.h>
#include <NevolutionX/render/renderer.h>
#include <NevolutionX/debug/outputLine.h>
#include <hal/xbox.h>
#include <SDL.h>


// lol I have no idea how to cpp
menu::menu(Renderer *rp, Font *fp, std::vector<xbeMenuItem> *gL, std::vector<xbeMenuItem> *aL) {
  r = rp;
  f = fp;
  gamesList = gL;
  appsList = aL;

  // Populate main menu
  mainMenu.push_back(menuItem("Games"));

  mainMenu.push_back(menuItem("Applications"));
  mainMenu.push_back(menuItem("Launch DVD"));
  mainMenu.push_back(menuItem("Settings"));
  mainMenu.push_back(menuItem("Reboot"));

  if (f->createTextures(mainMenu, r) != mainMenu.size()) {
    outputLine("Main menu textures could not be created.\n");
  }

  SDL_Texture* menuListTexture = r->compileList(mainMenu);
  if (menuListTexture == nullptr) {
    outputLine("Main menu list texture could not be compiled.\n");
  }
  r->drawMenuTexture(menuListTexture);

  mainMenuSelection = 0;
  currItem = 0;
  prevItem = 0;
  listSize = mainMenu.size();
}

void menu::goToMainMenu(menuItem *mI) {
  f->setPassive(mI, r);
  listSize = 5;
  currItem = 0;
  prevItem = 1;
  mainMenuSelection = 0;
}


void menu::up() {
  prevItem = currItem;
  if (currItem == 0) {
    currItem = listSize - 1;
  } else {
    --currItem;
  }
}

void menu::down() {
  prevItem = currItem;
  currItem = (currItem + 1) % listSize;
}

void menu::A() {
  switch (mainMenuSelection) {
  case 0:
    mainMenuSelection = currItem + 1;
    currItem = 0;
    prevItem = 1;
    break;
  case 1:
    if (currItem != (gamesList->size() - 1)) {
#ifdef NXDK
      XLaunchXBE(const_cast<char*>((*gamesList)[currItem].getXBEPath().c_str()));
#endif
    }
    goToMainMenu(&(*gamesList)[currItem]);
    break;
  case 2:
    if (currItem != (appsList->size() - 1)) {
#ifdef NXDK
      XLaunchXBE(const_cast<char*>((*appsList)[currItem].getXBEPath().c_str()));
#endif
    }
    goToMainMenu(&(*appsList)[currItem]);
    break;
  case 3:
#ifdef NXDK
    XLaunchXBE(const_cast<char*>("D:\\default.xbe"));
#endif
    mainMenuSelection = 0;
    break;
  default:
    break;
  }
}

void menu::back() {
  switch (mainMenuSelection) {
  case 0:
    break;
  default:
    goToMainMenu(&(*gamesList)[currItem]);
    break;
  }
}

int menu::render() {
  // FIXME: Loads of repetitions ahead - break out into functions
  switch (mainMenuSelection) {
  case 0:
    if (prevItem != currItem) {
      f->setPassive(&mainMenu[prevItem], r);
      f->setActive(&mainMenu[currItem], r);
      prevItem = currItem;
      r->updateMenuFrame(mainMenu);
    }
    break;
  case 1:
    if (prevItem != currItem) {
      f->setPassive(&gamesList->at(prevItem), r);
      f->setActive(&gamesList->at(currItem), r);
      prevItem = currItem;
      listSize = gamesList->size();
      r->updateMenuFrame((*gamesList), currItem);
    }
    break;
  case 2:
    if (prevItem != currItem) {
      f->setPassive(&appsList->at(prevItem), r);
      f->setActive(&appsList->at(currItem), r);
      prevItem = currItem;
      listSize = appsList->size();
      r->updateMenuFrame((*appsList), currItem);
    }
    break;
  case 4:
    // Settings menu. Not sure what we want/need here.
    // "it's a problem for the future".
    prevItem = 0;
    currItem = 3;
    mainMenuSelection = 0;
    break;
  case 5:
    prevItem = 0;
    currItem = 4;
    mainMenuSelection = 0;
    return -1;
  default:
    break;
  }
  return 0;
}
