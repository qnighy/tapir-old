#include <cstdlib>
#include <cstdio>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "sdl_misc.h"

SDL_Window *mainWindow;
SDL_Renderer *mainWindowRenderer;

void initSDL() {
  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
  TTF_Init();
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
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        // fprintf(stderr, "TODO: key\n");
        break;
      case SDL_WINDOWEVENT:
        // fprintf(stderr, "TODO: window event\n");
        break;
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEWHEEL:
        // don't do anything
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
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  exit(0);
}
