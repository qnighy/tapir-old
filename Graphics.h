#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <ruby.h>
#include <SDL.h>

#include "renderable.h"
#include "Viewport.h"

struct Graphics {
  static int width, height;
  static int frame_rate, frame_count, brightness;

  static int periodic_count;
  static Uint64 periodic_last;

  static void register_renderable(Renderable *r, Viewport *v);
  static void unregister_renderable(Renderable *r, Viewport *v);

  static void sort_renderables(std::vector<Renderable*> *rs);
  static void render_renderable(Renderable *r, SDL_Renderer *renderer);
};

extern VALUE rb_mGraphics;
extern void InitGraphics();

VALUE rb_graphics_update(VALUE self);
VALUE rb_graphics_wait(VALUE self, VALUE duration);
VALUE rb_graphics_fadeout(VALUE self, VALUE duration);
VALUE rb_graphics_fadein(VALUE self, VALUE duration);
VALUE rb_graphics_transition(int argc, VALUE *argv, VALUE self);
VALUE rb_graphics_snap_to_bitmap(VALUE self);
VALUE rb_graphics_frame_reset(VALUE self);
VALUE rb_graphics_width(VALUE self);
VALUE rb_graphics_height(VALUE self);
VALUE rb_graphics_resize_screen(VALUE self, VALUE width, VALUE height);
VALUE rb_graphics_play_movie(VALUE self, VALUE filename);
VALUE rb_graphics_frame_rate(VALUE self);
VALUE rb_graphics_set_frame_rate(VALUE self, VALUE frame_rate);
VALUE rb_graphics_frame_count(VALUE self);
VALUE rb_graphics_set_frame_count(VALUE self, VALUE frame_count);
VALUE rb_graphics_brightness(VALUE self);
VALUE rb_graphics_set_brightness(VALUE self, VALUE brightness);

#endif // GRAPHICS_H_INCLUDED
