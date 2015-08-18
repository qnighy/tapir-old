#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <ruby.h>

extern VALUE rb_cColor;
extern void Init_Color();

VALUE rb_color_new(double red, double green, double blue, double alpha);
VALUE rb_color_new2();

void rb_color_set(VALUE self, double red, double green, double blue, double alpha);
void rb_color_set2(VALUE self, VALUE other);
double rb_color_red(VALUE self);
void rb_color_set_red(VALUE self, double red);
double rb_color_green(VALUE self);
void rb_color_set_green(VALUE self, double green);
double rb_color_blue(VALUE self);
void rb_color_set_blue(VALUE self, double blue);
double rb_color_alpha(VALUE self);
void rb_color_set_alpha(VALUE self, double alpha);

#endif // COLOR_H_INCLUDED
