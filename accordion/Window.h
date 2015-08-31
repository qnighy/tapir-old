#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include <ruby.h>
#include <SDL.h>

#include "Bitmap.h"
#include "Color.h"
#include "Rect.h"
#include "Tone.h"
#include "Viewport.h"
#include "renderable.h"

struct Window {
  struct Renderable renderable_entry;
  VALUE rb_parent;

  Bitmap *windowskin, *contents;
  VALUE cursor_rect;
  Viewport *viewport;
  bool active, visible, arrows_visible, pause;
  int x, y, width, height, z, ox, oy;
  int padding, padding_bottom, opacity, back_opacity;
  int contents_opacity, openness;
  VALUE tone;

  bool is_disposed;

  void initialize(int x, int y, int width, int height);
  void initialize();
  void dispose();
  bool disposed();
  void update();
  void move(int x, int y, int width, int height);
  bool open_p();
  bool close_p();

  static Window *create(int x, int y, int width, int height);
  static Window *create();

  void render(
      SDL_Renderer *renderer,
      int rox, int roy, int rwidth, int rheight);
};

extern VALUE rb_cWindow;
extern void InitWindow();

Window *convertWindow(VALUE obj);
Window *convertWindowOrNil(VALUE obj);
VALUE exportWindow(Window *ptr);

#endif // WINDOW_H_INCLUDED
