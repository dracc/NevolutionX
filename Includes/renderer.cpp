#include "renderer.h"

#ifdef NXDK
extern "C" {
const extern int SCREEN_HEIGHT;
const extern int SCREEN_WIDTH;
}
#endif

Renderer::Renderer() {
  window = nullptr;
  renderer = nullptr;
#ifdef NXDK
  windowFlags = SDL_WINDOW_SHOWN;
  renderFlags = 0;
#else
  windowFlags = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE;
  renderFlags = SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED;
  SCREEN_WIDTH = 640;
  SCREEN_HEIGHT = 480;
#endif
  overscanCompX = SCREEN_WIDTH * 0.075;
  overscanCompY = SCREEN_HEIGHT * 0.075;
}

Renderer::~Renderer() {
  if (renderer != nullptr) {
    SDL_DestroyRenderer(renderer);
  }
  if (window != nullptr) {
    SDL_DestroyWindow(window);
  }
}

int Renderer::init() {
  window = SDL_CreateWindow("NevolutionX",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
  if (window == nullptr) {
    return 1;
  }
  renderer = SDL_CreateRenderer(window, -1, renderFlags);
  if (renderer == nullptr) {
    return 2;
  }
  setDrawColor();
  clear();
  return 0;
}

int Renderer::clear() {
  return SDL_RenderClear(renderer);
}

void Renderer::flip() {
  setDrawColor(0, 0, 0, 0xFF);
  SDL_RenderDrawRect(renderer, nullptr);
  setDrawColor();
  SDL_RenderPresent(renderer);
#ifdef NXDK
  XVideoWaitForVBlank();
#endif
}

int Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::drawTexture(SDL_Texture* tex, SDL_Rect &src, SDL_Rect &dst) {
  SDL_RenderCopy(renderer, tex, &src, &dst);
}

void Renderer::drawTexture(SDL_Texture* tex, SDL_Rect &dst) {
  SDL_RenderCopy(renderer, tex, nullptr, &dst);
}

void Renderer::drawTexture(SDL_Texture* tex, int x, int y) {
  SDL_Rect dst = {x, y, 0, 0};
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
  drawTexture(tex, dst);
}

void Renderer::drawMenuTexture(SDL_Texture* tex) {
  SDL_Rect dst = {overscanCompX, overscanCompY, 0, 0};
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
  drawTexture(tex, dst);
}

void Renderer::drawMenuTexture(SDL_Texture* tex, int numItems, int currItem) {
  SDL_Rect dst = {overscanCompX, overscanCompY, 0, 0};
  SDL_Rect *src = nullptr;
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
  int screenHeight = SCREEN_HEIGHT - (overscanCompY * 2);
  int rowHeight = dst.h / numItems;
  if (dst.h > screenHeight) {
    src = new SDL_Rect();
    src->w = dst.w;
    src->h = screenHeight;
    src->x = 0;
    src->y = fmin(fmax(rowHeight * currItem - screenHeight, 0), dst.h - screenHeight);
  }
  SDL_RenderCopy(renderer, tex, src, &dst);
}

SDL_Texture* Renderer::surfaceToTexture(SDL_Surface* surf) {
  return SDL_CreateTextureFromSurface(renderer, surf);
}

void Renderer::destroyTexture(SDL_Texture* tex) {
  SDL_DestroyTexture(tex);
  tex = nullptr;
}

SDL_Texture* Renderer::compileList(vector<xbeMenuItem> &l) {
  if (l.empty()) {
    return nullptr;
  }
  int h;
  SDL_QueryTexture(l[0].getTexture(), nullptr, nullptr, nullptr, &h);
  SDL_Texture *ret = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, 300,
                                       h*l.size());
  if (ret == nullptr) {
    return nullptr;
  }
  SDL_Rect dst = {0, 0, 0, h};
  SDL_SetRenderTarget(renderer, ret);
  setDrawColor();
  clear();
  for (size_t i = 0; i < l.size(); ++i) {
    if (l[i].getTexture() == nullptr) {
      continue;
    }
    SDL_QueryTexture(l[i].getTexture(), nullptr, nullptr, &dst.w, &dst.h);
    drawTexture(l[i].getTexture(), dst);
    dst.y += h;
  }
  SDL_SetRenderTarget(renderer, nullptr);
  return ret;
}

SDL_Texture* Renderer::compileList(vector<menuItem> &l) {
  if (l.empty()) {
    return nullptr;
  }
  int h;
  SDL_QueryTexture(l[0].getTexture(), nullptr, nullptr, nullptr, &h);
  SDL_Texture *ret = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, 300,
                                       h*l.size());
  if (ret == nullptr) {
    return nullptr;
  }
  SDL_Rect dst = {0, 0, 0, h};
  SDL_SetRenderTarget(renderer, ret);
  setDrawColor();
  clear();
  for (size_t i = 0; i < l.size(); ++i) {
    if (l[i].getTexture() == nullptr) {
      continue;
    }
    SDL_QueryTexture(l[i].getTexture(), nullptr, nullptr, &dst.w, &dst.h);
    drawTexture(l[i].getTexture(), dst);
    dst.y += h;
  }
  SDL_SetRenderTarget(renderer, nullptr);
  return ret;
}
