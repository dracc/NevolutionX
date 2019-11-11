#include <vector>
#include "findXBE.h"
#include "font.h"
#include "xbeMenuItem.h"
#include "menuItem.h"
#include "outputLine.h"
#include "renderer.h"
#include "subsystems.h"
#include "xpadinput.h"

#include <type_traits>
#include <threads.h>
#include <SDL.h>

#ifdef NXDK
#include <windows.h>
#endif

void goToMainMenu(menuItem *mI, Renderer *r, Font &f,
                  int &listSize, int &currItem, int &prevItem, int &mMS) {
  f.setPassive(mI, r);
  listSize = 5;
  currItem = 0;
  prevItem = 1;
  mMS = 0;
}

int main(void) {
  int init = init_systems();
  int mainMenuSelection = 0;
  std::vector<menuItem> mainMenu;
  std::vector<xbeMenuItem> gamesList;
  std::vector<xbeMenuItem> appsList;
  if (init == 0) {
    bool running = true;

    // Open our GameController
    SDL_GameController *sgc = SDL_GameControllerOpen(0);
    if (sgc == nullptr) {
      outputLine("Joystick Error: %s", SDL_GetError());
      Sleep(2000);
    }

    // Set a hint that we want to use our gamecontroller always
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    // Create the worker thread for populating the games list
    xbeFinderArg xfaG;
    xfaG.list = &gamesList;
    xfaG.path = "F:\\Games\\";
    thrd_t thrG;
    int thread_statusG = 1;
    thrd_create(&thrG, findXBE, &xfaG);

    // Create the worker thread for populating the applications list
    xbeFinderArg xfaA;
    xfaA.list = &appsList;
    xfaA.path = "F:\\Apps\\";
    thrd_t thrA;
    int thread_statusA = 1;
    thrd_create(&thrA, findXBE, &xfaA);

    // Create render system
    Renderer r;
    r.init(".");

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

    SDL_Event event;

    while (running) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          if (thread_statusG == 1) {
            thrd_join(thrG, &thread_statusG);
          }
          if (thread_statusA == 1) {
            thrd_join(thrA, &thread_statusA);
          }
          running = false;
          break;
        } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
          if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
            prevItem = currItem;
            if (currItem == 0) {
              currItem = listSize - 1;
            } else {
              --currItem;
            }
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
            prevItem = currItem;
            currItem = (currItem + 1) % listSize;
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
            switch (mainMenuSelection) {
            case 0:
              mainMenuSelection = currItem + 1;
              currItem = 0;
              prevItem = 1;
              break;
            case 1:
              if (currItem != (gamesList.size() - 1)) {
#ifdef NXDK
                XLaunchXBE(const_cast<char*>(gamesList[currItem].getXBEPath().c_str()));
#endif
              }
              goToMainMenu(&gamesList[currItem], &r, f, listSize, currItem, prevItem,
                           mainMenuSelection);
              break;
            case 2:
              if (currItem != (appsList.size() - 1)) {
#ifdef NXDK
                XLaunchXBE(const_cast<char*>(appsList[currItem].getXBEPath().c_str()));
#endif
              }
              goToMainMenu(&appsList[currItem], &r, f, listSize, currItem, prevItem,
                           mainMenuSelection);
              break;
            case 3:
#ifdef NXDK
              XLaunchXBE(const_cast<char*>("D:\\default.xbe"));
#endif
              mainMenuSelection = 0;
              break;
//            case 4:
//              break;
//            case 5:
//              break;
            default:
              break;
            }
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) {
            switch (mainMenuSelection) {
            case 0:
              break;
            default:
              goToMainMenu(&gamesList[currItem], &r, f, listSize, currItem, prevItem,
                           mainMenuSelection);
              break;
            }
          }
        }
      }
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
        break;
      case 1:
        if (thread_statusG == 1) {
          thrd_join(thrG, &thread_statusG);
          // FIXME: This check sucks.
          if (thread_statusG != 0) {
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
        break;
      case 2:
        if (thread_statusA == 1) {
          thrd_join(thrA, &thread_statusA);
          // FIXME: This check sucks.
          if (thread_statusA != 0) {
            outputLine("Apps list gathering failed.\n");
            mainMenuSelection = 0;
            break;
          }
          appsList.push_back(xbeMenuItem("<- back",""));
          ret = f.createTextures(appsList, &r);
          if (ret != appsList.size()) {
            outputLine("Games list textures could not be created.\n");
            mainMenuSelection = 0;
            break;
          }
          menuListTexture = r.compileList(appsList, 0);
          r.clear();
          r.drawBackground();
          r.drawMenuTexture(menuListTexture);
          r.flip();
          break;
        }
        if (prevItem != currItem) {
          f.setPassive(&appsList.at(prevItem), &r);
          f.setActive(&appsList.at(currItem), &r);
          prevItem = currItem;
          listSize = appsList.size();
          menuListTexture = r.compileList(appsList, currItem);
          r.clear();
          r.drawBackground();
          r.drawMenuTexture(menuListTexture);
          r.flip();
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
