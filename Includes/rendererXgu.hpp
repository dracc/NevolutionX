#ifndef RENDERERXGU_H
#define RENDERERXGU_H

#include <xgu.h>
#include <xgux.h>
#include <vector>

typedef struct Texture {
  uint16_t width;
  uint16_t height;
  uint16_t pitch;
  void* addr;
} Texture;

class RendererXgu {
public:
  RendererXgu();
  ~RendererXgu();

  int init();
  int init(const char* bg);
  void clear();
  void flip();

  int getWidth() const { return width; }
  int getHeight() const { return height; }

  int setDrawColor(uint8_t r = 0x40, uint8_t g = 0x40, uint8_t b = 0xE0, uint8_t a = 0x00);

  void drawTexture(Texture* tex, SDL_Rect& src, SDL_Rect& dst);
  void drawTexture(Texture* tex, SDL_Rect& dst);
  void drawTexture(Texture* tex, int x, int y);

  void fillRectangle(const SDL_Rect& dst);
  void fillRectangle(const SDL_FRect& dst);

  void drawBackground();
  void initTexture(std::string const& path);

private:
  Texture* background = nullptr;

  XguVec4 v_obj_rot = { 0, 0, 0, 1 };
  XguVec4 v_obj_scale = { 1, 1, 1, 1 };
  XguVec4 v_obj_pos = { 0, 0, 0, 1 };

  XguVec4 v_cam_pos = { 0, 0, 240, 1 };
  XguVec4 v_cam_rot = { 0, 0, 0, 1 };
  XguVec4 v_light_color = { 1, 1, 1, 1 };
  XguVec4 v_light_pos = { 0, 140, 0, 1 };
  XguVec4 v_light_ambient = { 1, 0, 0, 0 };

  XguMatrix4x4 m_model;
  XguMatrix4x4 m_view;
  XguMatrix4x4 m_proj;
  XguMatrix4x4 m_viewport;

  uint32_t color;
  int height = 0;
  int width = 0;
  int overscanCompX = 0;
  int overscanCompY = 0;
  size_t menuItemCount = 0;
  size_t lowerHalf = 0;
  size_t upperHalf = 0;
};

#endif
