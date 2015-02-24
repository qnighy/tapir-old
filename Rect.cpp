#include "Rect.h"

void Rect::initialize(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}
void Rect::initialize() {
  this->x = 0;
  this->y = 0;
  this->width = 0;
  this->height = 0;
}
void Rect::set(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}
void Rect::set(Rect *rect) {
  this->x = rect->x;
  this->y = rect->y;
  this->width = rect->width;
  this->height = rect->height;
}
void Rect::empty() {
  this->x = 0;
  this->y = 0;
  this->width = 0;
  this->height = 0;
}

static void rect_mark(Rect *);
static VALUE rect_alloc(VALUE klass);

static VALUE rb_rect_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_rect_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_rect_set(int argc, VALUE *argv, VALUE self);
static VALUE rb_rect_empty(VALUE self);

static VALUE rb_rect_x(VALUE self);
static VALUE rb_rect_set_x(VALUE self, VALUE x);
static VALUE rb_rect_y(VALUE self);
static VALUE rb_rect_set_y(VALUE self, VALUE y);
static VALUE rb_rect_width(VALUE self);
static VALUE rb_rect_set_width(VALUE self, VALUE width);
static VALUE rb_rect_height(VALUE self);
static VALUE rb_rect_set_height(VALUE self, VALUE height);

VALUE rb_cRect;
void InitRect() {
  rb_cRect = rb_define_class("Rect", rb_cObject);
  rb_define_alloc_func(rb_cRect, rect_alloc);
  rb_define_method(rb_cRect, "initialize",
      (VALUE(*)(...))rb_rect_initialize, -1);
  rb_define_method(rb_cRect, "initialize_copy",
      (VALUE(*)(...))rb_rect_initialize_copy, 1);
  rb_define_method(rb_cRect, "set",
      (VALUE(*)(...))rb_rect_set, -1);
  rb_define_method(rb_cRect, "empty",
      (VALUE(*)(...))rb_rect_empty, 0);
  rb_define_method(rb_cRect, "x",
      (VALUE(*)(...))rb_rect_x, 0);
  rb_define_method(rb_cRect, "x=",
      (VALUE(*)(...))rb_rect_set_x, 1);
  rb_define_method(rb_cRect, "y",
      (VALUE(*)(...))rb_rect_y, 0);
  rb_define_method(rb_cRect, "y=",
      (VALUE(*)(...))rb_rect_set_y, 1);
  rb_define_method(rb_cRect, "width",
      (VALUE(*)(...))rb_rect_width, 0);
  rb_define_method(rb_cRect, "width=",
      (VALUE(*)(...))rb_rect_set_width, 1);
  rb_define_method(rb_cRect, "height",
      (VALUE(*)(...))rb_rect_height, 0);
  rb_define_method(rb_cRect, "height=",
      (VALUE(*)(...))rb_rect_set_height, 1);
}

Rect *convertRect(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))rect_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Rect",
        rb_class2name(rb_obj_class(obj)));
  }
  Rect *ret;
  Data_Get_Struct(obj, Rect, ret);
  return ret;
}
Rect *convertRectOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertRect(obj);
}

VALUE newRect(int x, int y, int width, int height) {
  VALUE ret = rect_alloc(rb_cRect);
  convertRect(ret)->initialize(x, y, width, height);
  return ret;
}
VALUE newRect() {
  VALUE ret = rect_alloc(rb_cRect);
  convertRect(ret)->initialize();
  return ret;
}

static void rect_mark(Rect *) {}

static VALUE rect_alloc(VALUE klass) {
  Rect *ptr = ALLOC(Rect);
  VALUE ret = Data_Wrap_Struct(klass, rect_mark, -1, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_rect_initialize(int argc, VALUE *argv, VALUE self) {
  Rect *ptr = convertRect(self);
  switch(argc) {
    case 4:
      ptr->initialize(
          NUM2INT(argv[0]),
          NUM2INT(argv[1]),
          NUM2INT(argv[2]),
          NUM2INT(argv[3]));
      break;
    case 0:
      ptr->initialize();
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0 or 4)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_rect_initialize_copy(VALUE self, VALUE orig) {
  Rect *ptr = convertRect(self);
  Rect *orig_ptr = convertRect(orig);
  ptr->x = orig_ptr->x;
  ptr->y = orig_ptr->y;
  ptr->width = orig_ptr->width;
  ptr->height = orig_ptr->height;
  return Qnil;
}

static VALUE rb_rect_set(int argc, VALUE *argv, VALUE self) {
  Rect *ptr = convertRect(self);
  switch(argc) {
    case 4:
      ptr->set(
          NUM2INT(argv[0]),
          NUM2INT(argv[1]),
          NUM2INT(argv[2]),
          NUM2INT(argv[3]));
      break;
    case 1:
      ptr->set(convertRect(argv[0]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1 or 4)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_rect_empty(VALUE self) {
  Rect *ptr = convertRect(self);
  ptr->empty();
  return Qnil;
}

static VALUE rb_rect_x(VALUE self) {
  Rect *ptr = convertRect(self);
  return INT2NUM(ptr->x);
}
static VALUE rb_rect_set_x(VALUE self, VALUE x) {
  Rect *ptr = convertRect(self);
  ptr->x = NUM2INT(x);
  return x;
}
static VALUE rb_rect_y(VALUE self) {
  Rect *ptr = convertRect(self);
  return INT2NUM(ptr->y);
}
static VALUE rb_rect_set_y(VALUE self, VALUE y) {
  Rect *ptr = convertRect(self);
  ptr->y = NUM2INT(y);
  return y;
}
static VALUE rb_rect_width(VALUE self) {
  Rect *ptr = convertRect(self);
  return INT2NUM(ptr->width);
}
static VALUE rb_rect_set_width(VALUE self, VALUE width) {
  Rect *ptr = convertRect(self);
  ptr->width = NUM2INT(width);
  return width;
}
static VALUE rb_rect_height(VALUE self) {
  Rect *ptr = convertRect(self);
  return INT2NUM(ptr->height);
}
static VALUE rb_rect_set_height(VALUE self, VALUE height) {
  Rect *ptr = convertRect(self);
  ptr->height = NUM2INT(height);
  return height;
}
