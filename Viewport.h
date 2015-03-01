#ifndef VIEWPORT_H_INCLUDED
#define VIEWPORT_H_INCLUDED

#include <vector>

#include <ruby.h>
#include <SDL.h>

#include "Bitmap.h"
#include "Color.h"
#include "Rect.h"
#include "Tone.h"
#include "renderable.h"

struct Viewport {
  Renderable renderable_entry;
  VALUE rb_parent;

  Rect *rect;
  bool visible;
  int z, ox, oy;
  Color *color;
  Tone *tone;

  bool is_disposed;
  std::vector<Renderable*> *renderables;

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

  void render(
      SDL_Renderer *renderer,
      int rox, int roy, int rwidth, int rheight);
};

extern VALUE rb_cViewport;
extern void InitViewport();

Viewport *convertViewport(VALUE obj);
Viewport *convertViewportOrNil(VALUE obj);
VALUE exportViewport(Viewport *ptr);

#endif // VIEWPORT_H_INCLUDED
