#ifndef RECT_H_INCLUDED
#define RECT_H_INCLUDED

#include <ruby.h>

struct Rect {
  VALUE rb_parent;
  int x, y, width, height;
  void initialize(int x, int y, int width, int height);
  void initialize();
  void set(int x, int y, int width, int height);
  void set(Rect *rect);
  void empty();

  static Rect *create(int x, int y, int width, int height);
  static Rect *create();
};

extern VALUE rb_cRect;
extern void InitRect();

Rect *convertRect(VALUE obj);
Rect *convertRectOrNil(VALUE obj);

#endif // RECT_H_INCLUDED
