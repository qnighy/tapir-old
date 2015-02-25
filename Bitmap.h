#ifndef BITMAP_H_INCLUDED
#define BITMAP_H_INCLUDED

#include <ruby.h>
#include <SDL.h>

#include "Font.h"
#include "Rect.h"


struct Bitmap {
  VALUE rb_parent;
  SDL_Surface *surface;
  SDL_Texture *texture;
  Font *font;

  void initialize(const char *filename);
  void initialize(int width, int height);
  void dispose();
  bool disposed();
  int width();
  int height();
  Rect *rect();
  void blt(int x, int y, Bitmap *src_bitmap, Rect *src_rect,
      int opacity = 255);
  void stretch_blt(Rect *dest_rect, Bitmap *src_bitmap, Rect *src_rect,
      int opacity = 255);
  void fill_rect(int x, int y, int width, int height, Color *color);
  void fill_rect(Rect *rect, Color *color);
  void gradient_fill_rect(
      int x, int y, int width, int height, Color *color1, Color *color2,
      bool vertical = false);
  void gradient_fill_rect(
      Rect *rect, Color *color1, Color *color2,
      bool vertical = false);
  void clear();
  void clear_rect(int x, int y, int width, int height);
  void clear_rect(Rect *rect);
  Color *get_pixel(int x, int y);
  void set_pixel(int x, int y, Color *color);
  void hue_change(int hue);
  void blur();
  void radial_blur(int angle, int division);
  void draw_text(int x, int y, int width, int height, const char *str,
      int align = 0);
  void draw_text(Rect *rect, const char *str,
      int align = 0);
  Rect *text_size(const char *str);

  static Bitmap *create(const char *filename);
  static Bitmap *create(int width, int height);

  SDL_Texture *createTexture(SDL_Renderer *renderer);
  void invalidateTexture();
};

extern VALUE rb_cBitmap;
extern void InitBitmap();

Bitmap *convertBitmap(VALUE obj);
Bitmap *convertBitmapOrNil(VALUE obj);
VALUE exportBitmap(Bitmap *ptr);

#endif // BITMAP_H_INCLUDED
