#ifndef VIEWPORT_H_INCLUDED
#define VIEWPORT_H_INCLUDED

#include <ruby.h>
#include <SDL.h>

#include "Bitmap.h"
#include "Color.h"
#include "Rect.h"
#include "Tone.h"
#include "Viewport.h"

struct Viewport {
  VALUE rb_parent;

  Rect *rect;
  bool visible;
  int z, ox, oy;
  Color *color;
  Tone *tone;

  bool is_disposed;

  void initialize(int x, int y, int width, int height);
  void initialize(Rect *rect);
  void initialize();
  void dispose();
  bool disposed();
  void flash(Color *color, int duration);
  void update();

  static Viewport *create(int x, int y, int width, int height);
  static Viewport *create(Rect *rect);
  static Viewport *create();
};

extern VALUE rb_cViewport;
extern void InitViewport();

Viewport *convertViewport(VALUE obj);
Viewport *convertViewportOrNil(VALUE obj);
VALUE exportViewport(Viewport *ptr);

#endif // VIEWPORT_H_INCLUDED
