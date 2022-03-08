#ifndef NEVOLUTIONX_NXDKSDLGPU_H
#define NEVOLUTIONX_NXDKSDLGPU_H

#include <SDL.h>

// Patches the active SDL_VideoDevice with OpenGL callbacks that leverage pbgl.
//
// Must be called after SDL_Init.
void pbglConfigureSDLVideoDevice();

// Swaps the given SDL_Surface from RGB(A) to BGR(A) in place.
SDL_Surface* convertRGBToBGR(SDL_Surface* src);

#endif // NEVOLUTIONX_NXDKSDLGPU_H
