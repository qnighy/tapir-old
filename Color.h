#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <ruby.h>

struct Color {
  VALUE rb_parent;
  double red, green, blue, alpha;
  void initialize(double red, double green, double blue, double alpha = 255.0);
  void initialize();
  void set(double red, double green, double blue, double alpha = 255.0);
  void set(Color *color);
};

extern VALUE rb_cColor;
extern void InitColor();

Color *convertColor(VALUE obj);
Color *convertColorOrNil(VALUE obj);

Color *newColor(double red, double green, double blue, double alpha = 255.0);
Color *newColor();

#endif // COLOR_H_INCLUDED
