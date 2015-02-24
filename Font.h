#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include <ruby.h>

#include "Color.h"


struct Font {
  VALUE rb_parent;
  VALUE name;
  int size;
  bool bold, italic, shadow, outline;
  Color *color, *out_color;

  static VALUE default_name;
  static int default_size;
  static bool default_bold, default_italic, default_shadow, default_outline;
  static Color *default_color, *default_out_color;

  void initialize(VALUE name = default_name, int size = default_size);
  static bool exist(VALUE name);
  void set(Font *font);

  static Font *create(VALUE name = Font::default_name, int size = Font::default_size);
};

extern VALUE rb_cFont;
extern void InitFont();

Font *convertFont(VALUE obj);
Font *convertFontOrNil(VALUE obj);
VALUE exportFont(Font *ptr);

#endif // FONT_H_INCLUDED
