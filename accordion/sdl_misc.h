#ifndef SDL_MISC_H_INCLUDED
#define SDL_MISC_H_INCLUDED

#include <SDL.h>

extern SDL_Window *mainWindow;
extern SDL_Renderer *mainWindowRenderer;
void initSDL();
void pollEvent();
void quitSDL(int exitcode = 0);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
constexpr Uint32 amask = 0x000000ff;
constexpr Uint32 rmask = 0x0000ff00;
constexpr Uint32 gmask = 0x00ff0000;
constexpr Uint32 bmask = 0xff000000;
#else
constexpr Uint32 amask = 0xff000000;
constexpr Uint32 rmask = 0x00ff0000;
constexpr Uint32 gmask = 0x0000ff00;
constexpr Uint32 bmask = 0x000000ff;
#endif
constexpr int stdformat = SDL_PIXELFORMAT_ARGB8888;

#endif // SDL_MISC_H_INCLUDED
