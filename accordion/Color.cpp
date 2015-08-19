#include "Color.h"
#include "misc.h"

struct Color {
  double red, green, blue, alpha;
};

static Color *convertColor(VALUE obj);
static void color_mark(Color *);
static VALUE color_alloc(VALUE klass);

VALUE rb_color_new(double red, double green, double blue, double alpha) {
  VALUE ret = color_alloc(rb_cColor);
  Color *ptr = convertColor(ret);
  ptr->red = saturateDouble(red, 0.0, 255.0);
  ptr->green = saturateDouble(green, 0.0, 255.0);
  ptr->blue = saturateDouble(blue, 0.0, 255.0);
  ptr->alpha = saturateDouble(alpha, 0.0, 255.0);
  return ret;
}
VALUE rb_color_new2() {
  return rb_color_new(0.0, 0.0, 0.0, 0.0);
}

void rb_color_set(
    VALUE self, double red, double green, double blue, double alpha) {
  Color *ptr = convertColor(self);
  ptr->red = saturateDouble(red, 0.0, 255.0);
  ptr->green = saturateDouble(green, 0.0, 255.0);
  ptr->blue = saturateDouble(blue, 0.0, 255.0);
  ptr->alpha = saturateDouble(alpha, 0.0, 255.0);
}

void rb_color_set2(VALUE self, VALUE other) {
  Color *ptr = convertColor(self);
  Color *other_ptr = convertColor(other);
  ptr->red = other_ptr->red;
  ptr->green = other_ptr->green;
  ptr->blue = other_ptr->blue;
  ptr->alpha = other_ptr->alpha;
}

double rb_color_red(VALUE self) {
  Color *ptr = convertColor(self);
  return ptr->red;
}
void rb_color_set_red(VALUE self, double red) {
  Color *ptr = convertColor(self);
  ptr->red = red;
}
double rb_color_green(VALUE self) {
  Color *ptr = convertColor(self);
  return ptr->green;
}
void rb_color_set_green(VALUE self, double green) {
  Color *ptr = convertColor(self);
  ptr->green = green;
}
double rb_color_blue(VALUE self) {
  Color *ptr = convertColor(self);
  return ptr->blue;
}
void rb_color_set_blue(VALUE self, double blue) {
  Color *ptr = convertColor(self);
  ptr->blue = blue;
}
double rb_color_alpha(VALUE self) {
  Color *ptr = convertColor(self);
  return ptr->alpha;
}
void rb_color_set_alpha(VALUE self, double alpha) {
  Color *ptr = convertColor(self);
  ptr->alpha = alpha;
}

static VALUE rb_color_m_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_color_m_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_color_m_set(int argc, VALUE *argv, VALUE self);

static VALUE rb_color_m_red(VALUE self);
static VALUE rb_color_m_set_red(VALUE self, VALUE red);
static VALUE rb_color_m_green(VALUE self);
static VALUE rb_color_m_set_green(VALUE self, VALUE green);
static VALUE rb_color_m_blue(VALUE self);
static VALUE rb_color_m_set_blue(VALUE self, VALUE blue);
static VALUE rb_color_m_alpha(VALUE self);
static VALUE rb_color_m_set_alpha(VALUE self, VALUE alpha);
static VALUE rb_color_m_to_s(VALUE self);
static VALUE rb_color_m_old_load(VALUE self, VALUE s);
static VALUE rb_color_m_old_dump(VALUE self, VALUE lim);

VALUE rb_cColor;
void Init_Color() {
  rb_cColor = rb_define_class("Color", rb_cObject);
  rb_define_alloc_func(rb_cColor, color_alloc);
  rb_define_method(rb_cColor, "initialize",
      (VALUE(*)(...))rb_color_m_initialize, -1);
  rb_define_method(rb_cColor, "initialize_copy",
      (VALUE(*)(...))rb_color_m_initialize_copy, 1);
  rb_define_method(rb_cColor, "set",
      (VALUE(*)(...))rb_color_m_set, -1);
  rb_define_method(rb_cColor, "red",
      (VALUE(*)(...))rb_color_m_red, 0);
  rb_define_method(rb_cColor, "red=",
      (VALUE(*)(...))rb_color_m_set_red, 1);
  rb_define_method(rb_cColor, "green",
      (VALUE(*)(...))rb_color_m_green, 0);
  rb_define_method(rb_cColor, "green=",
      (VALUE(*)(...))rb_color_m_set_green, 1);
  rb_define_method(rb_cColor, "blue",
      (VALUE(*)(...))rb_color_m_blue, 0);
  rb_define_method(rb_cColor, "blue=",
      (VALUE(*)(...))rb_color_m_set_blue, 1);
  rb_define_method(rb_cColor, "alpha",
      (VALUE(*)(...))rb_color_m_alpha, 0);
  rb_define_method(rb_cColor, "alpha=",
      (VALUE(*)(...))rb_color_m_set_alpha, 1);

  rb_define_method(rb_cColor, "to_s",
      (VALUE(*)(...))rb_color_m_to_s, 0);
  rb_define_singleton_method(rb_cColor, "_load",
      (VALUE(*)(...))rb_color_m_old_load, 1);
  rb_define_method(rb_cColor, "_dump",
      (VALUE(*)(...))rb_color_m_old_dump, 1);
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

static void color_mark(Color *) {}

static VALUE color_alloc(VALUE klass) {
  Color *ptr = ALLOC(Color);
  VALUE ret = Data_Wrap_Struct(klass, color_mark, -1, ptr);
  return ret;
}

static VALUE rb_color_m_initialize(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      rb_color_set(self, 0.0, 0.0, 0.0, 0.0);
      break;
    case 1:
    case 2:
      rb_raise(rb_eArgError,
          "wrong number of arguments (3 for %d)", argc);
      break;
    case 3:
      rb_color_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          0.0);
      break;
    case 4:
      rb_color_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          NUM2DBL(argv[3]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_color_m_initialize_copy(VALUE self, VALUE orig) {
  rb_color_set2(self, orig);
  return Qnil;
}

static VALUE rb_color_m_set(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      rb_color_set(self, 0.0, 0.0, 0.0, 0.0);
      break;
    case 1:
      rb_color_set2(self, argv[0]);
      break;
    case 2:
      rb_raise(rb_eArgError,
          "wrong number of arguments (3 for %d)", argc);
      break;
    case 3:
      rb_color_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          0.0);
      break;
    case 4:
      rb_color_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          NUM2DBL(argv[3]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
  }
  return Qnil;
}

static VALUE rb_color_m_red(VALUE self) {
  return DBL2NUM(rb_color_red(self));
}
static VALUE rb_color_m_set_red(VALUE self, VALUE red) {
  rb_color_set_red(self, NUM2DBL(red));
  return red;
}
static VALUE rb_color_m_green(VALUE self) {
  return DBL2NUM(rb_color_green(self));
}
static VALUE rb_color_m_set_green(VALUE self, VALUE green) {
  rb_color_set_green(self, NUM2DBL(green));
  return green;
}
static VALUE rb_color_m_blue(VALUE self) {
  return DBL2NUM(rb_color_blue(self));
}
static VALUE rb_color_m_set_blue(VALUE self, VALUE blue) {
  rb_color_set_blue(self, NUM2DBL(blue));
  return blue;
}
static VALUE rb_color_m_alpha(VALUE self) {
  return DBL2NUM(rb_color_alpha(self));
}
static VALUE rb_color_m_set_alpha(VALUE self, VALUE alpha) {
  rb_color_set_alpha(self, NUM2DBL(alpha));
  return alpha;
}
static VALUE rb_color_m_to_s(VALUE self) {
  Color *ptr = convertColor(self);
  char s[50];
  snprintf(s, sizeof(s), "(%f, %f, %f, %f)",
      ptr->red,
      ptr->green,
      ptr->blue,
      ptr->alpha);
  return rb_str_new2(s);
}
static VALUE rb_color_m_old_load(VALUE, VALUE str) {
  VALUE ret = color_alloc(rb_cColor);
  Color *ptr = convertColor(ret);
  char *s = StringValuePtr(str);
  if(!s) return ret;
  ptr->red = saturateDouble(readDouble(s), 0.0, 255.0);
  ptr->green = saturateDouble(readDouble(s+8), 0.0, 255.0);
  ptr->blue = saturateDouble(readDouble(s+16), 0.0, 255.0);
  ptr->alpha = saturateDouble(readDouble(s+24), 0.0, 255.0);
  return ret;
}
static VALUE rb_color_m_old_dump(VALUE self, VALUE) {
  Color *ptr = convertColor(self);
  char s[32];
  writeDouble(s, ptr->red);
  writeDouble(s+8, ptr->green);
  writeDouble(s+16, ptr->blue);
  writeDouble(s+24, ptr->alpha);
  VALUE ret = rb_str_new(s, 32);
  return ret;
}
