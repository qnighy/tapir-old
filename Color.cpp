#include "Color.h"
#include "misc.h"

void Color::initialize(double red, double green, double blue, double alpha) {
  this->red = red;
  this->green = green;
  this->blue = blue;
  this->alpha = alpha;
}
void Color::initialize() {
  this->red = 0.0;
  this->green = 0.0;
  this->blue = 0.0;
  this->alpha = 0.0;
}
void Color::set(double red, double green, double blue, double alpha) {
  this->red = red;
  this->green = green;
  this->blue = blue;
  this->alpha = alpha;
}
void Color::set(Color *color) {
  this->red = color->red;
  this->green = color->green;
  this->blue = color->blue;
  this->alpha = color->alpha;
}

static void color_mark(Color *);
static VALUE color_alloc(VALUE klass);

static VALUE rb_color_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_color_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_color_set(int argc, VALUE *argv, VALUE self);

static VALUE rb_color_red(VALUE self);
static VALUE rb_color_set_red(VALUE self, VALUE red);
static VALUE rb_color_green(VALUE self);
static VALUE rb_color_set_green(VALUE self, VALUE green);
static VALUE rb_color_blue(VALUE self);
static VALUE rb_color_set_blue(VALUE self, VALUE blue);
static VALUE rb_color_alpha(VALUE self);
static VALUE rb_color_set_alpha(VALUE self, VALUE alpha);
static VALUE rb_color_old_load(VALUE self, VALUE s);
static VALUE rb_color_old_dump(VALUE self, VALUE lim);

VALUE rb_cColor;
void InitColor() {
  rb_cColor = rb_define_class("Color", rb_cObject);
  rb_define_alloc_func(rb_cColor, color_alloc);
  rb_define_method(rb_cColor, "initialize",
      (VALUE(*)(...))rb_color_initialize, -1);
  rb_define_method(rb_cColor, "initialize_copy",
      (VALUE(*)(...))rb_color_initialize_copy, 1);
  rb_define_method(rb_cColor, "set",
      (VALUE(*)(...))rb_color_set, -1);
  rb_define_method(rb_cColor, "red",
      (VALUE(*)(...))rb_color_red, 0);
  rb_define_method(rb_cColor, "red=",
      (VALUE(*)(...))rb_color_set_red, 1);
  rb_define_method(rb_cColor, "green",
      (VALUE(*)(...))rb_color_green, 0);
  rb_define_method(rb_cColor, "green=",
      (VALUE(*)(...))rb_color_set_green, 1);
  rb_define_method(rb_cColor, "blue",
      (VALUE(*)(...))rb_color_blue, 0);
  rb_define_method(rb_cColor, "blue=",
      (VALUE(*)(...))rb_color_set_blue, 1);
  rb_define_method(rb_cColor, "alpha",
      (VALUE(*)(...))rb_color_alpha, 0);
  rb_define_method(rb_cColor, "alpha=",
      (VALUE(*)(...))rb_color_set_alpha, 1);

  rb_define_singleton_method(rb_cColor, "_load",
      (VALUE(*)(...))rb_color_old_load, 1);
  rb_define_method(rb_cColor, "_dump",
      (VALUE(*)(...))rb_color_old_dump, 1);
}

Color *convertColor(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))color_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Color",
        rb_class2name(rb_obj_class(obj)));
  }
  Color *ret;
  Data_Get_Struct(obj, Color, ret);
  return ret;
}
Color *convertColorOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertColor(obj);
}

Color *newColor(double red, double green, double blue, double alpha) {
  VALUE ret = color_alloc(rb_cColor);
  Color *ptr = convertColor(ret);
  ptr->initialize(red, green, blue, alpha);
  return ptr;
}
Color *newColor() {
  VALUE ret = color_alloc(rb_cColor);
  Color *ptr = convertColor(ret);
  ptr->initialize();
  return ptr;
}

static void color_mark(Color *) {}

static VALUE color_alloc(VALUE klass) {
  Color *ptr = ALLOC(Color);
  VALUE ret = Data_Wrap_Struct(klass, color_mark, -1, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_color_initialize(int argc, VALUE *argv, VALUE self) {
  Color *ptr = convertColor(self);
  switch(argc) {
    case 4:
      ptr->initialize(
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          NUM2DBL(argv[3]));
      break;
    case 3:
      ptr->initialize(
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]));
      break;
    case 0:
      ptr->initialize();
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0 or 3..4)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_color_initialize_copy(VALUE self, VALUE orig) {
  Color *ptr = convertColor(self);
  Color *orig_ptr = convertColor(orig);
  ptr->red = orig_ptr->red;
  ptr->green = orig_ptr->green;
  ptr->blue = orig_ptr->blue;
  ptr->alpha = orig_ptr->alpha;
  return Qnil;
}

static VALUE rb_color_set(int argc, VALUE *argv, VALUE self) {
  Color *ptr = convertColor(self);
  switch(argc) {
    case 4:
      ptr->set(
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          NUM2DBL(argv[3]));
      break;
    case 3:
      ptr->set(
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]));
      break;
    case 1:
      ptr->set(convertColor(argv[0]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1 or 3..4)", argc);
      break;
  }
  return Qnil;
}

static VALUE rb_color_red(VALUE self) {
  Color *ptr = convertColor(self);
  return DBL2NUM(ptr->red);
}
static VALUE rb_color_set_red(VALUE self, VALUE red) {
  Color *ptr = convertColor(self);
  ptr->red = NUM2DBL(red);
  return red;
}
static VALUE rb_color_green(VALUE self) {
  Color *ptr = convertColor(self);
  return DBL2NUM(ptr->green);
}
static VALUE rb_color_set_green(VALUE self, VALUE green) {
  Color *ptr = convertColor(self);
  ptr->green = NUM2DBL(green);
  return green;
}
static VALUE rb_color_blue(VALUE self) {
  Color *ptr = convertColor(self);
  return DBL2NUM(ptr->blue);
}
static VALUE rb_color_set_blue(VALUE self, VALUE blue) {
  Color *ptr = convertColor(self);
  ptr->blue = NUM2DBL(blue);
  return blue;
}
static VALUE rb_color_alpha(VALUE self) {
  Color *ptr = convertColor(self);
  return DBL2NUM(ptr->alpha);
}
static VALUE rb_color_set_alpha(VALUE self, VALUE alpha) {
  Color *ptr = convertColor(self);
  ptr->alpha = NUM2DBL(alpha);
  return alpha;
}
static VALUE rb_color_old_load(VALUE, VALUE str) {
  VALUE ret = color_alloc(rb_cColor);
  Color *ptr = convertColor(ret);
  char *s = StringValuePtr(str);
  if(!s) return ret;
  ptr->red = readDouble(s);
  ptr->green = readDouble(s+8);
  ptr->blue = readDouble(s+16);
  ptr->alpha = readDouble(s+24);
  return ret;
}
static VALUE rb_color_old_dump(VALUE self, VALUE) {
  Color *ptr = convertColor(self);
  char s[32];
  writeDouble(s, ptr->red);
  writeDouble(s+8, ptr->green);
  writeDouble(s+16, ptr->blue);
  writeDouble(s+24, ptr->alpha);
  VALUE ret = rb_str_new(s, 32);
  return ret;
}
