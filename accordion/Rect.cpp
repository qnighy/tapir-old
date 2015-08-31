#include "Rect.h"
#include "misc.h"

struct Rect {
  int x, y, width, height;
};

bool isRect(VALUE obj);
Rect *convertRect(VALUE obj);
static void rect_mark(Rect *);
static VALUE rect_alloc(VALUE klass);

VALUE rb_rect_new(int x, int y, int width, int height) {
  VALUE ret = rect_alloc(rb_cRect);
  struct Rect *ptr = convertRect(ret);
  ptr->x = x;
  ptr->y = y;
  ptr->width = width;
  ptr->height = height;
  return ret;
}

VALUE rb_rect_new2(void) {
  return rb_rect_new(0, 0, 0, 0);
}

bool rb_rect_equal(VALUE self, VALUE other) {
  struct Rect *ptr = convertRect(self);
  struct Rect *other_ptr = convertRect(other);
  return
    ptr->x == other_ptr->x &&
    ptr->y == other_ptr->y &&
    ptr->width == other_ptr->width &&
    ptr->height == other_ptr->height;
}

void rb_rect_set(VALUE self, int newx, int newy, int newwidth, int newheight) {
  struct Rect *ptr = convertRect(self);
  ptr->x = newx;
  ptr->y = newy;
  ptr->width = newwidth;
  ptr->height = newheight;
}

void rb_rect_set2(VALUE self, VALUE other) {
  struct Rect *ptr = convertRect(self);
  struct Rect *other_ptr = convertRect(other);
  ptr->x = other_ptr->x;
  ptr->y = other_ptr->y;
  ptr->width = other_ptr->width;
  ptr->height = other_ptr->height;
}

int rb_rect_x(VALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->x;
}
void rb_rect_set_x(VALUE self, int newval) {
  struct Rect *ptr = convertRect(self);
  ptr->x = newval;
}
int rb_rect_y(VALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->y;
}
void rb_rect_set_y(VALUE self, int newval) {
  struct Rect *ptr = convertRect(self);
  ptr->y = newval;
}
int rb_rect_width(VALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->width;
}
void rb_rect_set_width(VALUE self, int newval) {
  struct Rect *ptr = convertRect(self);
  ptr->width = newval;
}
int rb_rect_height(VALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->height;
}
void rb_rect_set_height(VALUE self, int newval) {
  struct Rect *ptr = convertRect(self);
  ptr->height = newval;
}

static VALUE rb_rect_m_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_rect_m_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_rect_m_equal(VALUE self, VALUE other);

static VALUE rb_rect_m_set(int argc, VALUE *argv, VALUE self);
static VALUE rb_rect_m_empty(VALUE self);

static VALUE rb_rect_m_x(VALUE self);
static VALUE rb_rect_m_set_x(VALUE self, VALUE x);
static VALUE rb_rect_m_y(VALUE self);
static VALUE rb_rect_m_set_y(VALUE self, VALUE y);
static VALUE rb_rect_m_width(VALUE self);
static VALUE rb_rect_m_set_width(VALUE self, VALUE width);
static VALUE rb_rect_m_height(VALUE self);
static VALUE rb_rect_m_set_height(VALUE self, VALUE height);
static VALUE rb_rect_m_to_s(VALUE self);
static VALUE rb_rect_m_old_load(VALUE self, VALUE s);
static VALUE rb_rect_m_old_dump(VALUE self, VALUE lim);

VALUE rb_cRect;

void Init_Rect() {
  rb_cRect = rb_define_class("Rect", rb_cObject);
  rb_define_alloc_func(rb_cRect, rect_alloc);
  rb_define_method(rb_cRect, "initialize",
      (VALUE(*)(...))rb_rect_m_initialize, -1);
  rb_define_private_method(rb_cRect, "initialize_copy",
      (VALUE(*)(...))rb_rect_m_initialize_copy, 1);
  rb_define_method(rb_cRect, "==",
      (VALUE(*)(...))rb_rect_m_equal, 1);
  rb_define_method(rb_cRect, "set",
      (VALUE(*)(...))rb_rect_m_set, -1);
  rb_define_method(rb_cRect, "empty",
      (VALUE(*)(...))rb_rect_m_empty, 0);
  rb_define_method(rb_cRect, "x",
      (VALUE(*)(...))rb_rect_m_x, 0);
  rb_define_method(rb_cRect, "x=",
      (VALUE(*)(...))rb_rect_m_set_x, 1);
  rb_define_method(rb_cRect, "y",
      (VALUE(*)(...))rb_rect_m_y, 0);
  rb_define_method(rb_cRect, "y=",
      (VALUE(*)(...))rb_rect_m_set_y, 1);
  rb_define_method(rb_cRect, "width",
      (VALUE(*)(...))rb_rect_m_width, 0);
  rb_define_method(rb_cRect, "width=",
      (VALUE(*)(...))rb_rect_m_set_width, 1);
  rb_define_method(rb_cRect, "height",
      (VALUE(*)(...))rb_rect_m_height, 0);
  rb_define_method(rb_cRect, "height=",
      (VALUE(*)(...))rb_rect_m_set_height, 1);
  rb_define_method(rb_cRect, "to_s",
      (VALUE(*)(...))rb_rect_m_to_s, 0);
  rb_define_singleton_method(rb_cRect, "_load", (VALUE(*)(...))rb_rect_m_old_load, 1);
  rb_define_method(rb_cRect, "_dump", (VALUE(*)(...))rb_rect_m_old_dump, 1);
}

bool isRect(VALUE obj) {
  if(TYPE(obj) != T_DATA) return false;
  return RDATA(obj)->dmark == (void(*)(void*))rect_mark;
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

static void rect_mark(Rect *) {}

static VALUE rect_alloc(VALUE klass) {
  Rect *ptr = ALLOC(Rect);
  VALUE ret = Data_Wrap_Struct(klass, rect_mark, -1, ptr);
  return ret;
}

static VALUE rb_rect_m_initialize(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      rb_rect_set(self, 0, 0, 0, 0);
      break;
    case 1:
    case 2:
    case 3:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
    case 4:
      rb_rect_set(
          self, NUM2INT(argv[0]), NUM2INT(argv[1]),
          NUM2INT(argv[2]), NUM2INT(argv[3]));
      break;
    default:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_rect_m_initialize_copy(VALUE self, VALUE orig) {
  rb_rect_set2(self, orig);
  return Qnil;
}

static VALUE rb_rect_m_equal(VALUE self, VALUE other) {
  if(!isRect(other)) return Qfalse;
  return rb_rect_equal(self, other) ? Qtrue : Qfalse;
}

static VALUE rb_rect_m_set(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (1 for %d)", argc);
      break;
    case 1:
      rb_rect_set2(self, argv[0]);
      break;
    case 2:
    case 3:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
    case 4:
      rb_rect_set(
          self, NUM2INT(argv[0]), NUM2INT(argv[1]),
          NUM2INT(argv[2]), NUM2INT(argv[3]));
      break;
    default:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
  }
  return self;
}
static VALUE rb_rect_m_empty(VALUE self) {
  rb_rect_set(self, 0, 0, 0, 0);
  return self;
}

static VALUE rb_rect_m_x(VALUE self) {
  return INT2NUM(rb_rect_x(self));
}
static VALUE rb_rect_m_set_x(VALUE self, VALUE newval) {
  rb_rect_set_x(self, NUM2INT(newval));
  return newval;
}
static VALUE rb_rect_m_y(VALUE self) {
  return INT2NUM(rb_rect_y(self));
}
static VALUE rb_rect_m_set_y(VALUE self, VALUE newval) {
  rb_rect_set_y(self, NUM2INT(newval));
  return newval;
}
static VALUE rb_rect_m_width(VALUE self) {
  return INT2NUM(rb_rect_width(self));
}
static VALUE rb_rect_m_set_width(VALUE self, VALUE newval) {
  rb_rect_set_width(self, NUM2INT(newval));
  return newval;
}
static VALUE rb_rect_m_height(VALUE self) {
  return INT2NUM(rb_rect_height(self));
}
static VALUE rb_rect_m_set_height(VALUE self, VALUE newval) {
  rb_rect_set_height(self, NUM2INT(newval));
  return newval;
}

static VALUE rb_rect_m_to_s(VALUE self) {
  struct Rect *ptr = convertRect(self);
  char s[50];
  snprintf(s, sizeof(s), "(%d, %d, %d, %d)",
      ptr->x, ptr->y, ptr->width, ptr->height);
  return rb_str_new2(s);
}

static VALUE rb_rect_m_old_load(VALUE klass, VALUE str) {
  VALUE ret = rect_alloc(rb_cRect);
  struct Rect *ptr = convertRect(ret);
  char *s = StringValuePtr(str);
  if(!s) return ret;
  ptr->x = readInt(s);
  ptr->y = readInt(s+4);
  ptr->width = readInt(s+8);
  ptr->height = readInt(s+12);
  return ret;
}

static VALUE rb_rect_m_old_dump(VALUE self, VALUE limit) {
  struct Rect *ptr = convertRect(self);
  char s[16];
  writeInt(s, ptr->x);
  writeInt(s+4, ptr->y);
  writeInt(s+8, ptr->width);
  writeInt(s+12, ptr->height);
  VALUE ret = rb_str_new(s, 16);
  return ret;
}
