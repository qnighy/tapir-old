/**
 * @file Rect.h
 * @brief It defines Rect class.
 * @author Masaki Hara
 */

#ifndef RECT_H_INCLUDED
#define RECT_H_INCLUDED

#include <stdbool.h>

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE rb_cRect;

extern void Init_Rect(void);

VALUE rb_rect_new(int x, int y, int width, int height);
VALUE rb_rect_new2(void);
bool rb_rect_equal(VALUE self, VALUE other);
void rb_rect_set(VALUE self, int newx, int newy, int newwidth, int newheight);
void rb_rect_set2(VALUE self, VALUE other);
int rb_rect_x(VALUE self);
void rb_rect_set_x(VALUE self, int newx);
int rb_rect_y(VALUE self);
void rb_rect_set_y(VALUE self, int newy);
int rb_rect_width(VALUE self);
void rb_rect_set_width(VALUE self, int newwidth);
int rb_rect_height(VALUE self);
void rb_rect_set_height(VALUE self, int newheight);

#ifdef __cplusplus
}
#endif

#endif // RECT_H_INCLUDED
