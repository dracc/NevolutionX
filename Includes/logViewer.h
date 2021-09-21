#ifndef NEVOLUTIONX_INCLUDES_LOGVIEWER_H_
#define NEVOLUTIONX_INCLUDES_LOGVIEWER_H_

#include <SDL.h>
#include "renderer.h"
#include "subApp.h"

class logViewer : public SubApp {
public:
  explicit logViewer(Renderer& r);

  void render(Font& font) override;

  void onUpPressed() override { scroll(-1); };
  void onDownPressed() override { scroll(1); }
  void onBackPressed() override { quit(); }
  void onBPressed() override { quit(); }

private:
  void scroll(int delta);
  void quit();

  int endIndex;

  Renderer& renderer;
  SDL_FRect renderArea;
};

#endif // NEVOLUTIONX_INCLUDES_LOGVIEWER_H_
