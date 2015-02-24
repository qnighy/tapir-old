#include <ruby.h>
#include <SDL.h>

#include "Graphics.h"
#include "sdl_misc.h"

VALUE rb_mGraphics;

void InitGraphics() {
  rb_mGraphics = rb_define_module("Graphics");
  rb_define_singleton_method(rb_mGraphics, "update",
      (VALUE(*)(...))rb_graphics_update, 0);
}

VALUE rb_graphics_update(VALUE self) {
  pollEvent();
  SDL_RenderFillRect(mainWindowRenderer, NULL);
  SDL_RenderPresent(mainWindowRenderer);
  SDL_Delay(1000/60);
  return Qnil;
}
