#include <algorithm>

#include <ruby.h>
#include <SDL.h>

#include "Sprite.h"
#include "Viewport.h"
#include "Window.h"
#include "Graphics.h"
#include "sdl_misc.h"
#include "misc.h"
#include "renderable.h"

void Graphics::register_renderable(Renderable *r, Viewport *v) {
  // fprintf(stderr, "Renderable Registered\n");
  std::vector<Renderable*> *rs = v ? v->renderables : global_renderables;
  rs->push_back(r);
}
void Graphics::unregister_renderable(Renderable *r, Viewport *v) {
  // fprintf(stderr, "Renderable Unregistered\n");
  std::vector<Renderable*> *rs = v ? v->renderables : global_renderables;
  rs->resize(std::remove(rs->begin(), rs->end(), r) - rs->begin());
}
void Graphics::sort_renderables(std::vector<Renderable*> *rs) {
  std::sort(rs->begin(), rs->end(), [](Renderable *a, Renderable *b) {
      if(a->z != b->z) return a->z < b->z;
      if(a->y != b->y) return a->y < b->y;
      return a->renderable_id < b->renderable_id;
  });
}
void Graphics::render_renderable(Renderable *r, SDL_Renderer *renderer) {
  switch(r->type) {
    case RenderableType::SPRITE:
      ((Sprite*)r)->render(renderer);
      break;
    case RenderableType::VIEWPORT:
      ((Viewport*)r)->render(renderer);
      break;
    case RenderableType::WINDOW:
      ((Window*)r)->render(renderer);
      break;
    default:
      break;
  }
}

int Graphics::width;
int Graphics::height;
int Graphics::frame_rate;
int Graphics::frame_count;
int Graphics::brightness;

VALUE rb_mGraphics;

void InitGraphics() {
  current_renderable_id = 0;
  global_renderables = new std::vector<Renderable*>();

  Graphics::width = 544;
  Graphics::height = 416;
  Graphics::frame_rate = 60;
  Graphics::frame_count = 0;
  Graphics::brightness = 255;

  rb_mGraphics = rb_define_module("Graphics");
  rb_define_singleton_method(rb_mGraphics, "update",
      (VALUE(*)(...))rb_graphics_update, 0);
  rb_define_singleton_method(rb_mGraphics, "wait",
      (VALUE(*)(...))rb_graphics_wait, 1);
  rb_define_singleton_method(rb_mGraphics, "fadeout",
      (VALUE(*)(...))rb_graphics_fadeout, 1);
  rb_define_singleton_method(rb_mGraphics, "fadein",
      (VALUE(*)(...))rb_graphics_fadein, 1);
  rb_define_singleton_method(rb_mGraphics, "transition",
      (VALUE(*)(...))rb_graphics_transition, -1);
  rb_define_singleton_method(rb_mGraphics, "snap_to_bitmap",
      (VALUE(*)(...))rb_graphics_snap_to_bitmap, 0);
  rb_define_singleton_method(rb_mGraphics, "frame_reset",
      (VALUE(*)(...))rb_graphics_frame_reset, 0);
  rb_define_singleton_method(rb_mGraphics, "width",
      (VALUE(*)(...))rb_graphics_width, 0);
  rb_define_singleton_method(rb_mGraphics, "height",
      (VALUE(*)(...))rb_graphics_height, 0);
  rb_define_singleton_method(rb_mGraphics, "resize_screen",
      (VALUE(*)(...))rb_graphics_resize_screen, 2);
  rb_define_singleton_method(rb_mGraphics, "play_movie",
      (VALUE(*)(...))rb_graphics_play_movie, 1);
  rb_define_singleton_method(rb_mGraphics, "frame_rate",
      (VALUE(*)(...))rb_graphics_frame_rate, 0);
  rb_define_singleton_method(rb_mGraphics, "frame_rate=",
      (VALUE(*)(...))rb_graphics_set_frame_rate, 1);
  rb_define_singleton_method(rb_mGraphics, "frame_count",
      (VALUE(*)(...))rb_graphics_frame_count, 0);
  rb_define_singleton_method(rb_mGraphics, "frame_count=",
      (VALUE(*)(...))rb_graphics_set_frame_count, 1);
  rb_define_singleton_method(rb_mGraphics, "brightness",
      (VALUE(*)(...))rb_graphics_brightness, 0);
  rb_define_singleton_method(rb_mGraphics, "brightness=",
      (VALUE(*)(...))rb_graphics_set_brightness, 1);
}

VALUE rb_graphics_update(VALUE) {
  pollEvent();
  ++Graphics::frame_count;
  SDL_SetRenderDrawBlendMode(mainWindowRenderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(mainWindowRenderer, 0, 0, 0, 255);
  SDL_RenderFillRect(mainWindowRenderer, NULL);
  Graphics::sort_renderables(global_renderables);
  for(Renderable *r : *global_renderables) {
    Graphics::render_renderable(r, mainWindowRenderer);
  }
  SDL_RenderPresent(mainWindowRenderer);
  SDL_Delay(1000/Graphics::frame_rate);
  return Qnil;
}
VALUE rb_graphics_wait(VALUE self, VALUE duration) {
  int duration_int = NUM2INT(duration);
  for(int i = 0; i < duration_int; ++i) {
    rb_graphics_update(self);
  }
  return Qnil;
}
VALUE rb_graphics_fadeout(VALUE self, VALUE duration) {
  fprintf(stderr, "TODO: Graphics::fadeout\n");
  int duration_int = NUM2INT(duration);
  for(int i = 0; i < duration_int; ++i) {
    rb_graphics_update(self);
  }
  Graphics::brightness = 0;
  return Qnil;
}
VALUE rb_graphics_fadein(VALUE self, VALUE duration) {
  fprintf(stderr, "TODO: Graphics::fadein\n");
  int duration_int = NUM2INT(duration);
  for(int i = 0; i < duration_int; ++i) {
    rb_graphics_update(self);
  }
  Graphics::brightness = 255;
  return Qnil;
}
VALUE rb_graphics_transition(int argc, VALUE *argv, VALUE self) {
  if(argc > 3) {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for 0..3)", argc);
  }
  int duration = 10;
  if(argc > 0) duration = NUM2INT(argv[0]);
  std::string filename = "";
  if(argc > 1 && argv[1] != Qnil) filename = StringValueCStr(argv[1]);
  int vague = 40;
  if(argc > 2) vague = saturate(NUM2INT(argv[2]), 0, 255);

  fprintf(stderr, "TODO: Graphics::transition\n");
  for(int i = 0; i < duration; ++i) {
    rb_graphics_update(self);
  }
  return Qnil;
}
VALUE rb_graphics_snap_to_bitmap(VALUE) {
  fprintf(stderr, "TODO: Graphics::snap_to_bitmap\n");
  return exportBitmap(Bitmap::create(Graphics::width, Graphics::height));
}
VALUE rb_graphics_frame_reset(VALUE) {
  fprintf(stderr, "TODO: Graphics::frame_reset\n");
}
VALUE rb_graphics_width(VALUE) {
  return INT2NUM(Graphics::width);
}
VALUE rb_graphics_height(VALUE) {
  return INT2NUM(Graphics::height);
}
VALUE rb_graphics_resize_screen(VALUE, VALUE width, VALUE height) {
  Graphics::width = NUM2INT(width);
  Graphics::height = NUM2INT(height);
  SDL_DestroyRenderer(mainWindowRenderer);
  SDL_SetWindowSize(mainWindow, Graphics::width, Graphics::height);
  mainWindowRenderer = SDL_CreateRenderer(mainWindow, -1,
      SDL_RENDERER_ACCELERATED);
}
VALUE rb_graphics_play_movie(VALUE, VALUE filename) {
  fprintf(stderr, "TODO: Graphics::play_movie\n");
}
VALUE rb_graphics_frame_rate(VALUE) {
  return INT2NUM(Graphics::frame_rate);
}
VALUE rb_graphics_set_frame_rate(VALUE, VALUE frame_rate) {
  Graphics::frame_rate = saturate(NUM2INT(frame_rate), 10, 120);
  return frame_rate;
}
VALUE rb_graphics_frame_count(VALUE) {
  return INT2NUM(Graphics::frame_count);
}
VALUE rb_graphics_set_frame_count(VALUE, VALUE frame_count) {
  Graphics::frame_count = NUM2INT(frame_count);
  return frame_count;
}
VALUE rb_graphics_brightness(VALUE) {
  return INT2NUM(Graphics::brightness);
}
VALUE rb_graphics_set_brightness(VALUE, VALUE brightness) {
  Graphics::brightness = saturate(NUM2INT(brightness), 0, 255);
  return brightness;
}
