#include "renderer.h"

#include "outputLine.h"
#include <algorithm>
#include <math.h>

// One line of text with the default font is 31 pixels high.
// FIXME: Should probably be dynamic and dependent of font settings.
const unsigned int FONT_TEX_SIZE = 31;

Renderer::Renderer() {
#ifdef NXDK
  VIDEO_MODE xmode = XVideoGetMode();
  height = xmode.height;
  width = xmode.width;
  windowFlags = SDL_WINDOW_SHOWN;
#else
  height = 640;
  width = 480;
  windowFlags = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE;
  renderFlags = SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED;
#endif
  overscanCompX = width * 0.075;
  overscanCompY = height * 0.075;
  menuItemCount = (height - (overscanCompY * 2)) / FONT_TEX_SIZE;
  lowerHalf = menuItemCount/2;
  upperHalf = ceil(menuItemCount/2.0);
}

Renderer::~Renderer() {
  if (renderer != nullptr) {
    SDL_DestroyRenderer(renderer);
  }
  if (window != nullptr) {
    SDL_DestroyWindow(window);
  }
  if (background != nullptr) {
    SDL_DestroyTexture(background);
  }
}

int Renderer::init() {
  window = SDL_CreateWindow("NevolutionX",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            width, height, windowFlags);
  if (window == nullptr) {
    return 1;
  }
  renderer = SDL_CreateRenderer(window, -1, renderFlags);
  if (renderer == nullptr) {
    return 2;
  }
  SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);
  setDrawColor();
  clear();
  return 0;
}

int Renderer::init(const char* bgpath) {
  int ret = init();
  if (ret != 0) {
    return ret;
  }
  char* bgname = (char*)malloc(strlen(bgpath)+10);
  sprintf(bgname, "%s/%d.bmp", bgpath, height);
  SDL_Surface *bgsurf = SDL_LoadBMP(bgname);
  free(bgname);
  if (bgsurf == nullptr) {
    outputLine("Creating background surface failed.\n");
    return 3;
  }
  background = SDL_CreateTextureFromSurface(renderer, bgsurf);
  SDL_FreeSurface(bgsurf);
  if (background == nullptr) {
    outputLine("Creating background texture failed.\n");
    return 4;
  }
  return ret;
}

int Renderer::clear() {
  int ret = SDL_RenderClear(renderer);
  return ret;
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

void Renderer::blitSurface(SDL_Surface* bg, SDL_Surface* fg, int offset) {
  SDL_Rect dst = {offset, offset, fg->w, fg->h};
  SDL_SetSurfaceBlendMode(fg, SDL_BLENDMODE_BLEND);
  SDL_BlitSurface(fg, NULL, bg, &dst);
}

void Renderer::drawBackground() {
  if (background != nullptr) {
    drawTexture(background, 0, 0);
  }
}

void Renderer::drawMenuTexture(SDL_Texture* tex) {
  SDL_Rect dst = {overscanCompX, overscanCompY, 0, 0};
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
  drawTexture(tex, dst);
  SDL_DestroyTexture(tex);
  tex = nullptr;
}

void Renderer::drawMenuTexture(SDL_Texture* tex, int numItems, int currItem) {
  SDL_Rect dst = {overscanCompX, overscanCompY, 0, 0};
  SDL_Rect *src = nullptr;
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
  int screenHeight = height - (overscanCompY * 2);
  int rowHeight = dst.h / numItems;
  if (dst.h > screenHeight) {
    src = new SDL_Rect();
    src->w = dst.w;
    src->h = screenHeight;
    src->x = 0;
    src->y = std::min(std::max((rowHeight * currItem) - (screenHeight / 2), 0), dst.h - screenHeight);
    dst.h = screenHeight;
  }
  SDL_RenderCopy(renderer, tex, src, &dst);
  if (src != nullptr) {
    free(src);
  }
  destroyTexture(tex);
}

SDL_Texture* Renderer::surfaceToTexture(SDL_Surface* surf) {
  return SDL_CreateTextureFromSurface(renderer, surf);
}

void Renderer::destroyTexture(SDL_Texture* tex) {
  SDL_DestroyTexture(tex);
  tex = nullptr;
}

SDL_Texture* Renderer::compileList(std::vector<xbeMenuItem> &l, size_t currItem) {
  if (l.empty()) {
    return nullptr;
  }
  int h;
  size_t i, j;
  if (l.size() <= menuItemCount) {
    i = 0;
    j = l.size();
  } else {
      if (currItem > (l.size() - lowerHalf)) {
      i = l.size() - menuItemCount;
      j = l.size();
    } else if (currItem < upperHalf) {
      i = 0;
      j = menuItemCount;
    } else {
      i = currItem - upperHalf;
      j = currItem + lowerHalf;
    }
  }
  SDL_QueryTexture(l[0].getTexture(), nullptr, nullptr, nullptr, &h);
  SDL_Texture *ret = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, 300,
                                       h*menuItemCount);
  if (ret == nullptr) {
    return nullptr;
  }
  SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
  SDL_Rect dst = {0, 0, 0, h};
  SDL_SetRenderTarget(renderer, ret);
  setDrawColor();
  clear();
  for (; i < j; ++i) {
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

SDL_Texture* Renderer::compileList(std::vector<xbeMenuItem> &l) {
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
  SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
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

SDL_Texture* Renderer::compileList(std::vector<menuItem> &l) {
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
  SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
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
