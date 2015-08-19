#include <algorithm>

#include <ruby.h>
#include <SDL.h>
#include <SDL_timer.h>

#include "Sprite.h"
#include "Tilemap.h"
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
void Graphics::force_unregister_renderable(Renderable *r) {
  switch(r->type) {
    case RenderableType::SPRITE:
      ((Sprite*)r)->viewport = nullptr;
      break;
    case RenderableType::VIEWPORT:
      break;
    case RenderableType::WINDOW:
      ((Window*)r)->viewport = nullptr;
      break;
    case RenderableType::TILEMAP:
      ((Tilemap*)r)->viewport = nullptr;
      break;
    case RenderableType::TILEMAP_SUB:
      ((TilemapSub*)r)->parent->viewport = nullptr;
      break;
    default:
      break;
  }
}
void Graphics::render_renderable(
    Renderable *r, SDL_Renderer *renderer,
    int rox, int roy, int rwidth, int rheight) {
  switch(r->type) {
    case RenderableType::SPRITE:
      ((Sprite*)r)->render(renderer, rox, roy, rwidth, rheight);
      break;
    case RenderableType::VIEWPORT:
      ((Viewport*)r)->render(renderer, rox, roy, rwidth, rheight);
      break;
    case RenderableType::WINDOW:
      ((Window*)r)->render(renderer, rox, roy, rwidth, rheight);
      break;
    case RenderableType::TILEMAP:
      ((Tilemap*)r)->render(renderer, rox, roy, rwidth, rheight, false);
      break;
    case RenderableType::TILEMAP_SUB:
      ((TilemapSub*)r)->parent->render(
        renderer, rox, roy, rwidth, rheight, true);
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

int Graphics::periodic_count;
Uint64 Graphics::periodic_last;

VALUE rb_mGraphics;

void InitGraphics() {
  current_renderable_id = 0;
  global_renderables = new std::vector<Renderable*>();

  Graphics::width = 544;
  Graphics::height = 416;
  Graphics::frame_rate = 60;
  Graphics::frame_count = 0;
  Graphics::brightness = 255;

  Graphics::periodic_count = 0;
  Graphics::periodic_last = SDL_GetPerformanceCounter();

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

static Uint64 internal_last;
static Uint64 update_counter() {
  Uint64 internal_current = SDL_GetPerformanceCounter();
  Uint64 ret = internal_current - internal_last;
  internal_last = internal_current;
  return ret;
}

static int internal_counter = 0;
static Uint64 time1 = 0;
static Uint64 time2 = 0;
static Uint64 time3 = 0;
VALUE rb_graphics_update(VALUE) {
  time1 += update_counter();
  pollEvent();
  ++Graphics::frame_count;
  SDL_SetRenderDrawBlendMode(mainWindowRenderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(mainWindowRenderer, 0, 0, 0, 255);
  SDL_RenderFillRect(mainWindowRenderer, NULL);
  Graphics::sort_renderables(global_renderables);
  for(Renderable *r : *global_renderables) {
    Graphics::render_renderable(
        r, mainWindowRenderer, 0, 0, Graphics::width, Graphics::height);
  }
  SDL_SetRenderDrawColor(
      mainWindowRenderer, 0, 0, 0, 255-Graphics::brightness);
  SDL_RenderFillRect(mainWindowRenderer, NULL);
  SDL_RenderPresent(mainWindowRenderer);
  time2 += update_counter();
  {
    ++Graphics::periodic_count;
    Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 elapsed = SDL_GetPerformanceCounter() - Graphics::periodic_last;
    Uint64 desired = freq * Graphics::periodic_count / Graphics::frame_rate;
    if(desired > elapsed) {
      SDL_Delay((desired-elapsed)*1000/freq);
    } else {
      Uint64 d = elapsed - desired - freq*5/Graphics::frame_rate;
      if((Sint64)d > 0) {
        Graphics::periodic_last += d/3;
      }
    }
    if(Graphics::periodic_count >= Graphics::frame_rate) {
      Graphics::periodic_count -= Graphics::frame_rate;
      Graphics::periodic_last += freq;
    }
  }
  time3 += update_counter();
  if(internal_counter%100==0) {
    Uint64 freq = SDL_GetPerformanceFrequency();
    printf("ruby: %gms, render: %gms, wait: %gms, %gfps\n",
        time1*0.01/freq*1000,
        time2*0.01/freq*1000,
        time3*0.01/freq*1000,
        freq/((time1+time2+time3)*0.01));
    internal_counter = 0;
    time1 = 0;
    time2 = 0;
    time3 = 0;
  }
  ++internal_counter;
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
  int duration_int = NUM2INT(duration);
  int old_brightness = Graphics::brightness;
  for(int i = 0; i < duration_int; ++i) {
    Graphics::brightness = old_brightness*(duration_int-i)/duration;
    rb_graphics_update(self);
  }
  Graphics::brightness = 0;
  return Qnil;
}
VALUE rb_graphics_fadein(VALUE self, VALUE duration) {
  int duration_int = NUM2INT(duration);
  int old_brightness = 255-Graphics::brightness;
  for(int i = 0; i < duration_int; ++i) {
    Graphics::brightness = 255-old_brightness*(duration_int-i)/duration;
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
  if(argc > 2) vague = saturateInt(NUM2INT(argv[2]), 0, 255);

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
  Graphics::periodic_count = 0;
  Graphics::periodic_last = SDL_GetPerformanceCounter();
  return Qnil;
}
VALUE rb_graphics_width(VALUE) {
  return INT2NUM(Graphics::width);
}
VALUE rb_graphics_height(VALUE) {
  return INT2NUM(Graphics::height);
}
VALUE rb_graphics_resize_screen(VALUE, VALUE width, VALUE height) {
  // TODO: Graphics::resize_screen: check correctness
  Graphics::width = NUM2INT(width);
  Graphics::height = NUM2INT(height);
  SDL_SetWindowSize(mainWindow, Graphics::width, Graphics::height);
  // SDL_RenderSetLogicalSize(
  //     mainWindowRenderer, Graphics::width, Graphics::height);
  return Qnil;
}
VALUE rb_graphics_play_movie(VALUE, VALUE filename) {
  fprintf(stderr, "TODO: Graphics::play_movie\n");
  return Qnil;
}
VALUE rb_graphics_frame_rate(VALUE) {
  return INT2NUM(Graphics::frame_rate);
}
VALUE rb_graphics_set_frame_rate(VALUE, VALUE frame_rate) {
  Graphics::frame_rate = saturateInt(NUM2INT(frame_rate), 10, 120);
  Graphics::periodic_count = 0;
  Graphics::periodic_last = SDL_GetPerformanceCounter();
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
  Graphics::brightness = saturateInt(NUM2INT(brightness), 0, 255);
  return brightness;
}
