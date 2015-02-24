#ifndef SDL_MISC_H_INCLUDED
#define SDL_MISC_H_INCLUDED

#include <SDL.h>

extern SDL_Window *mainWindow;
extern SDL_Renderer *mainWindowRenderer;
void initSDL();
void pollEvent();
void quitSDL();

#endif // SDL_MISC_H_INCLUDED
