#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include <ruby.h>
#include <SDL.h>

#include "Bitmap.h"
#include "Color.h"
#include "Rect.h"
#include "Tone.h"
#include "Viewport.h"
#include "renderable.h"

struct Sprite {
  struct Renderable renderable_entry;
  VALUE rb_parent;

  Bitmap *bitmap;
  Rect *src_rect;
  Viewport *viewport;
  bool visible;
  int x, y, z, ox, oy;
  double zoom_x, zoom_y, angle;
  int wave_amp, wave_length, wave_speed;
  double wave_phase;
  bool mirror;
  int bush_depth, bush_opacity, opacity, blend_type;
  VALUE color;
  VALUE tone;

  bool is_disposed;

  void initialize(Viewport *viewport = nullptr);
  void dispose();
  bool disposed();
  void flash(VALUE color, int duration);
  void update();
  int width();
  int height();

  static Sprite *create(Viewport *viewport = nullptr);

  void render(
      SDL_Renderer *renderer,
      int rox, int roy, int rwidth, int rheight);
};

extern VALUE rb_cSprite;
extern void InitSprite();

Sprite *convertSprite(VALUE obj);
Sprite *convertSpriteOrNil(VALUE obj);
VALUE exportSprite(Sprite *ptr);

#endif // SPRITE_H_INCLUDED
