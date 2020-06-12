#include <threads.h>
#include <vector>
#include <type_traits>
#include <SDL.h>

#include <NevolutionX/sys/findXBE.h>
#include <NevolutionX/render/font.h>
#include <NevolutionX/menu/xbeMenuItem.h>
#include <NevolutionX/menu/menuItem.h>
#include <NevolutionX/menu/menu.h>
#include <NevolutionX/debug/outputLine.h>
#include <NevolutionX/render/renderer.h>
#include <NevolutionX/sys/subsystems.h>
#include <NevolutionX/ftp/ftpServer.h>

#ifdef NXDK
#include <hal/xbox.h>
#include <hal/video.h>
#include <windows.h>
#endif


// creates worker thread for searching through the FS
int thread_search_for(thrd_t *thr,
                      const char * path,
                      std::vector<xbeMenuItem> *list,
                      xbeFinderArg *xfa) {
  xfa->list = list;
  xfa->path = path;
  thrd_create(thr, findXBE, &xfa);
  return 1;
}

int thread_operate_network(thrd_t *thrNet) {
  thrd_create(thrNet, init_network, NULL);
  return 1;
}


int main(void) {
  thrd_t thrG;
  xbeFinderArg xfaG;
  int thread_statusG = 0;
  std::vector<xbeMenuItem> gamesList;

  thrd_t thrA;
  xbeFinderArg xfaA;
  int thread_statusA = 0;
  std::vector<xbeMenuItem> appsList;

  thrd_t thrNet;
  int network_status = 0;

  int running = 1;

  if (init_drives()) {
    // Handle error
  }

  if (init_video()) {
    // Handle error
  }

  // Create the worker thread for populating the games list
  thread_statusG = thread_search_for(&thrG, "F:\\Games\\", &gamesList, &xfaG);

  // Create the worker thread for populating the applications list
  thread_statusA = thread_search_for(&thrA, "F:\\Apps\\", &appsList, &xfaA);

  // Brings the NIC up with static ip or dhcp
  // if successfull, continues to run as FTP server
  network_status = thread_operate_network(&thrNet);

  if (init_input()) {
    // Handle error
  }

  // Create render system
  Renderer r;
  r.init("D:");

  // Create font because do it
  Font f("D:\\vegur.ttf");

  r.drawBackground();
  r.flip();

  SDL_Event event;

  thrd_join(thrA, &thread_statusA);
  // FIXME: This check sucks.
  if (thread_statusA != 0) {
    outputLine("Apps list gathering failed.\n");
    running = 0;
  }
  appsList.push_back(xbeMenuItem("<- back", ""));
  if (f.createTextures(appsList, &r) != appsList.size()) {
    outputLine("Games list textures could not be created.\n");
    running = 0;
  }
  r.updateMenuFrame(appsList, 0);

  thrd_join(thrG, &thread_statusG);
  // FIXME: This check sucks.
  if (thread_statusG != 0) {
    outputLine("Games list gathering failed.\n");
    running = 0;
  }
  gamesList.push_back(xbeMenuItem("<- back", ""));
  if (f.createTextures(gamesList, &r) != gamesList.size()) {
    outputLine("Games list textures could not be created.\n");
    running = 0;
  }
  r.updateMenuFrame(gamesList, 0);

  menu Menu = menu(&r, &f, &gamesList, &appsList);

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
        switch (event.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
          Menu.up();
          break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
          Menu.down();
          break;
        case SDL_CONTROLLER_BUTTON_A:
          Menu.A();
          break;
        case SDL_CONTROLLER_BUTTON_B:
        case SDL_CONTROLLER_BUTTON_BACK:
          Menu.back();
          break;
        default:
          break;
        }
      }
    }
    if (Menu.render() != 0)
      running = 0;
#ifdef NXDK
    // Let's not hog CPU for nothing.
    XVideoWaitForVBlank();
    SwitchToThread();
#endif
  }

  shutdown_systems(3);
  return 0;
}
