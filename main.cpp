#include <vector.hpp>
#include "findXBE.h"
#include "font.h"
#include "xbeMenuItem.h"
#include "menuItem.h"
#include "outputLine.h"
#include "renderer.h"
#include "subsystems.h"
#include "xpadinput.h"

#include <threads.h>

void goToMainMenu(menuItem *mI, Renderer *r, Font &f,
                  int &listSize, int &currItem, int &prevItem, int &mMS) {
  f.setPassive(mI, r);
  listSize = 5;
  currItem = 0;
  prevItem = 1;
  mMS = 0;
}

#ifdef NXDK
#include <hal/xbox.h>
// FIXME: This will probably be extraneous soon (SDL2 has a fix upstream)
extern "C" void _exit(int rc) {}

// Create some sloppy replacements for needed CPP functions
void * __cdecl operator new(unsigned int size) { return malloc(size); }
void __cdecl operator delete(void* itm) { free(itm); }
#endif

int main(void) {
  int init = init_systems();
  int mainMenuSelection = 0;
  vector<menuItem> mainMenu;
  vector<xbeMenuItem> gamesList;
  if (init == 0) {
    bool running = true;
    // Create the worker thread for populating the games list
    xbeFinderArg xfa;
    xfa.list = &gamesList;
//    xfa.path = const_cast<char*>("C:\\");
    xfa.path = const_cast<char*>("F:\\Games\\");
    thrd_t thr;
    int thread_status = 1;
    thrd_create(&thr, findXBE, &xfa);

    // Create render system
    Renderer r;
    r.init("480p.bmp");

    // Create font because do it
    Font f("vegur.ttf");

    // Populate main menu
    mainMenu.push_back(menuItem("Games"));
    mainMenu.push_back(menuItem("Applications"));
    mainMenu.push_back(menuItem("Launch DVD"));
    mainMenu.push_back(menuItem("Settings"));
    mainMenu.push_back(menuItem("Reboot"));

    size_t ret = f.createTextures(mainMenu, &r);
    if (ret != mainMenu.size()) {
      outputLine("Main menu textures could not be created.\n");
    }

    SDL_Texture* menuListTexture = r.compileList(mainMenu);
    if (menuListTexture == nullptr) {
      outputLine("Main menu list texture could not be compiled.\n");
    }
    r.drawBackground();
    r.drawMenuTexture(menuListTexture);
    r.flip();
    int currItem = 0, prevItem = 0, listSize = mainMenu.size();

    while (running) {
      // FIXME: Abstract the input- and menu navigation process
#ifdef NXDK
      XInput_GetEvents();
      for (size_t i = 0; i < XInputGetPadCount(); ++i) {
        if (getDigitalKeyDown(&g_Pads[i], XPAD_DPAD_UP)) {
          prevItem = currItem;
          if (currItem == 0) {
            currItem = listSize - 1;
          } else {
            --currItem;
          }
        }
        if (getDigitalKeyDown(&g_Pads[i], XPAD_DPAD_DOWN)) {
          prevItem = currItem;
          currItem = (currItem + 1) % listSize;
        }
      }
#endif
      // FIXME: Loads of repetitions ahead - break out into functions
      switch (mainMenuSelection) {
      case 0:
        if (prevItem != currItem) {
          f.setPassive(&mainMenu[prevItem], &r);
          f.setActive(&mainMenu[currItem], &r);
          prevItem = currItem;
          menuListTexture = r.compileList(mainMenu);
          r.clear();
          r.drawBackground();
          r.drawMenuTexture(menuListTexture);
          r.flip();
        }
#ifdef NXDK
        if (getAnalogKeyDown(&g_Pads[0], XPAD_A)) {
          mainMenuSelection = currItem + 1;
          currItem = 0;
          prevItem = 1;
        }
#endif
        break;
      case 1:
        if (thread_status == 1) {
          thrd_join(thr, &thread_status);
          // FIXME: This check sucks.
          if (thread_status != 0) {
            outputLine("Games list gathering failed.\n");
            mainMenuSelection = 0;
            break;
          }
          gamesList.push_back(xbeMenuItem("<- back",""));
          ret = f.createTextures(gamesList, &r);
          if (ret != gamesList.size()) {
            outputLine("Games list textures could not be created.\n");
            mainMenuSelection = 0;
            break;
          }
          menuListTexture = r.compileList(gamesList, 0);
          r.clear();
          r.drawBackground();
          r.drawMenuTexture(menuListTexture);
          r.flip();
          break;
        }
        if (prevItem != currItem) {
          f.setPassive(&gamesList.at(prevItem), &r);
          f.setActive(&gamesList.at(currItem), &r);
          prevItem = currItem;
          listSize = gamesList.size();
          menuListTexture = r.compileList(gamesList, currItem);
          r.clear();
          r.drawBackground();
          r.drawMenuTexture(menuListTexture);
          r.flip();
        }
#ifdef NXDK
        if (getAnalogKeyDown(&g_Pads[0], XPAD_A)) {
          if (currItem != (gamesList.size() - 1)) {
            XLaunchXBE(const_cast<char*>(gamesList[currItem].getXBEPath()));
          }
          goToMainMenu(&gamesList[currItem], &r, f, listSize, currItem, prevItem,
                       mainMenuSelection);
          break;
        }
        if (getAnalogKeyDown(&g_Pads[0], XPAD_B)) {
          goToMainMenu(&gamesList[currItem], &r, f, listSize, currItem, prevItem,
                       mainMenuSelection);
          break;
        }
#endif
        break;
      case 2:
        // FIXME: Essentially the same deal as case 1;
        // ought to be trivial to create a function for this mess.
        mainMenuSelection = 0;
        break;
      case 3:
#ifdef NXDK
        XLaunchXBE(const_cast<char*>("D:\\default.xbe"));
#endif
        mainMenuSelection = 0;
        break;
      case 4:
        // Settings menu. Not sure what we want/need here.
        // "it's a problem for the future".
        prevItem = 0;
        currItem = 3;
        mainMenuSelection = 0;
        break;
      case 5:
        running = false;
        prevItem = 0;
        currItem = 4;
        mainMenuSelection = 0;
        break;
      default:
        break;
      }
    }
  }
  shutdown_systems(init);
  return init;
}
