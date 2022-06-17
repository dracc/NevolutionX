#include "rendererXgu.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <hal/video.h>
#include <algorithm>
#include <cmath>
#include "infoLog.hpp"

// One line of text with the default font is 31 pixels high.
// FIXME: Should probably be dynamic and dependent of font settings.
const unsigned int FONT_TEX_SIZE = 36;

RendererXgu::RendererXgu() {
  VIDEO_MODE xmode = XVideoGetMode();
  height = xmode.height;
  width = xmode.width;
  overscanCompX = width * 0.075;
  overscanCompY = height * 0.075;
  menuItemCount = (height - (overscanCompY * 2)) / FONT_TEX_SIZE;
  lowerHalf = menuItemCount / 2;
  upperHalf = ceil(menuItemCount / 2.0);
}

RendererXgu::~RendererXgu() {
  if (background != nullptr) {
    delete (background->addr);
    delete (background);
  }
}

int RendererXgu::init() {
  background = init("D:\\720.png");
  return 0;
}

int RendererXgu::init(const char* bgpath) {
  background = initTexture(bgpath);
  pb_show_front_screen();
  setDrawColor();
  clear();
  return 0;
}

void RendererXgu::clear() {
  pb_erase_depth_stencil_buffer(0, 0, width, height);
  pb_fill(0, 0, width, height, 0xff202020);
  pb_erase_text_screen();
}

void RendererXgu::flip() {
  setDrawColor(0, 0, 0, 0xFF);
  SDL_RenderDrawRect(renderer, nullptr);
  setDrawColor();
  SDL_RenderPresent(renderer);
#ifdef NXDK
  XVideoWaitForVBlank();
#endif
}

int RendererXgu::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  color = r << 24 | g << 16 | b << 8 | a;
  return 1;
}

void RendererXgu::drawTexture(SDL_Texture* tex, SDL_Rect& src, SDL_Rect& dst) {
  SDL_RenderCopy(renderer, tex, &src, &dst);
}

void RendererXgu::drawTexture(SDL_Texture* tex, SDL_Rect& dst) {
  SDL_RenderCopy(renderer, tex, nullptr, &dst);
}

void RendererXgu::drawTexture(SDL_Texture* tex, int x, int y) {
  SDL_Rect dst = { x, y, 0, 0 };
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
  drawTexture(tex, dst);
}

void RendererXgu::fillRectangle(const SDL_Rect& dst) {
  SDL_RenderFillRect(renderer, &dst);
}

void RendererXgu::fillRectangle(const SDL_FRect& dst) {
  SDL_RenderFillRectF(renderer, &dst);
}

void RendererXgu::blitSurface(SDL_Surface* bg, SDL_Surface* fg, int offset) {
  SDL_Rect dst = { offset, offset, fg->w, fg->h };
  SDL_SetSurfaceBlendMode(fg, SDL_BLENDMODE_BLEND);
  SDL_BlitSurface(fg, NULL, bg, &dst);
}

void RendererXgu::drawBackground() {
  if (background != nullptr) {
    drawTexture(background, 0, 0);
  }
}

Texture* RendererXgu::initTexture(std::string const& path) {
  Texture* ret = new Texture();
  SDL_Surface* output = IMG_Load(path.c_str());
  if (output) {
    SDL_Surface* bgsurf = SDL_ConvertSurfaceFormat(output, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(output);
    ret.width = bgsurf->w;
    ret.height = bgsurf->h;
    ret.pitch = bgsurf->pitch;
    ret.addr = MmAllocateContiguousMemoryEx(ret.pitch * ret.height, 0, 0x03FFAFFF, 0,
                                            PAGE_WRITECOMBINE | PAGE_READWRITE);
    memcpy(ret.addr, bgsurf->pixels, bgsurf->pitch * bgsurf->h);
    SDL_FreeSurface(bgsurf);
  } else {
    ret.width = 1;
    ret.height = 1;
    ret.pitch = 4;
    ret.addr = MmAllocateContiguousMemoryEx(ret.pitch * ret.height, 0, 0x03FFAFFF, 0,
                                            PAGE_WRITECOMBINE | PAGE_READWRITE);
    *(uint32_t*)ret.addr = 0xFF804080;
  }

  return ret;
}
