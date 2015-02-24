#include <cstdlib>
#include <cstdio>
#include <SDL.h>

#include "sdl_misc.h"

SDL_Window *mainWindow;
SDL_Renderer *mainWindowRenderer;

void initSDL() {
  SDL_Init(SDL_INIT_EVERYTHING);
  mainWindow = SDL_CreateWindow(
      "Hoge", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      544, 416, SDL_WINDOW_SHOWN);
  mainWindowRenderer = SDL_CreateRenderer(mainWindow, -1,
      SDL_RENDERER_ACCELERATED);
}

void pollEvent() {
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0) {
    switch(e.type) {
      case SDL_QUIT:
        quitSDL();
        break;
      default:
        fprintf(stderr, "Unknown event\n");
        break;
    }
  }
}

void quitSDL() {
  SDL_DestroyRenderer(mainWindowRenderer);
  SDL_DestroyWindow(mainWindow);
  SDL_Quit();
  exit(0);
}
