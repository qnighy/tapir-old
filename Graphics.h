#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <ruby.h>

extern VALUE rb_mGraphics;
extern void InitGraphics();

VALUE rb_graphics_update(VALUE self);

#endif // GRAPHICS_H_INCLUDED
