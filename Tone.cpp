#include "Tone.h"
#include "misc.h"

void Tone::initialize(double red, double green, double blue, double gray) {
  this->red = red;
  this->green = green;
  this->blue = blue;
  this->gray = gray;
}
void Tone::initialize() {
  this->red = 0.0;
  this->green = 0.0;
  this->blue = 0.0;
  this->gray = 0.0;
}
void Tone::set(double red, double green, double blue, double gray) {
  this->red = red;
  this->green = green;
  this->blue = blue;
  this->gray = gray;
}
void Tone::set(Tone *tone) {
  this->red = tone->red;
  this->green = tone->green;
  this->blue = tone->blue;
  this->gray = tone->gray;
}

static void tone_mark(Tone *);
static VALUE tone_alloc(VALUE klass);

static VALUE rb_tone_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_tone_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_tone_set(int argc, VALUE *argv, VALUE self);

static VALUE rb_tone_red(VALUE self);
static VALUE rb_tone_set_red(VALUE self, VALUE red);
static VALUE rb_tone_green(VALUE self);
static VALUE rb_tone_set_green(VALUE self, VALUE green);
static VALUE rb_tone_blue(VALUE self);
static VALUE rb_tone_set_blue(VALUE self, VALUE blue);
static VALUE rb_tone_gray(VALUE self);
static VALUE rb_tone_set_gray(VALUE self, VALUE gray);
static VALUE rb_tone_old_load(VALUE self, VALUE s);
static VALUE rb_tone_old_dump(VALUE self, VALUE lim);

VALUE rb_cTone;
void InitTone() {
  rb_cTone = rb_define_class("Tone", rb_cObject);
  rb_define_alloc_func(rb_cTone, tone_alloc);
  rb_define_method(rb_cTone, "initialize",
      (VALUE(*)(...))rb_tone_initialize, -1);
  rb_define_method(rb_cTone, "initialize_copy",
      (VALUE(*)(...))rb_tone_initialize_copy, 1);
  rb_define_method(rb_cTone, "set",
      (VALUE(*)(...))rb_tone_set, -1);
  rb_define_method(rb_cTone, "red",
      (VALUE(*)(...))rb_tone_red, 0);
  rb_define_method(rb_cTone, "red=",
      (VALUE(*)(...))rb_tone_set_red, 1);
  rb_define_method(rb_cTone, "green",
      (VALUE(*)(...))rb_tone_green, 0);
  rb_define_method(rb_cTone, "green=",
      (VALUE(*)(...))rb_tone_set_green, 1);
  rb_define_method(rb_cTone, "blue",
      (VALUE(*)(...))rb_tone_blue, 0);
  rb_define_method(rb_cTone, "blue=",
      (VALUE(*)(...))rb_tone_set_blue, 1);
  rb_define_method(rb_cTone, "gray",
      (VALUE(*)(...))rb_tone_gray, 0);
  rb_define_method(rb_cTone, "gray=",
      (VALUE(*)(...))rb_tone_set_gray, 1);

  rb_define_singleton_method(rb_cTone, "_load",
      (VALUE(*)(...))rb_tone_old_load, 1);
  rb_define_method(rb_cTone, "_dump",
      (VALUE(*)(...))rb_tone_old_dump, 1);
}

Tone *convertTone(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))tone_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Tone",
        rb_class2name(rb_obj_class(obj)));
  }
  Tone *ret;
  Data_Get_Struct(obj, Tone, ret);
  return ret;
}
Tone *convertToneOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertTone(obj);
}

Tone *newTone(double red, double green, double blue, double gray) {
  VALUE ret = tone_alloc(rb_cTone);
  Tone *ptr = convertTone(ret);
  ptr->initialize(red, green, blue, gray);
  return ptr;
}
Tone *newTone() {
  VALUE ret = tone_alloc(rb_cTone);
  Tone *ptr = convertTone(ret);
  ptr->initialize();
  return ptr;
}

static void tone_mark(Tone *) {}

static VALUE tone_alloc(VALUE klass) {
  Tone *ptr = ALLOC(Tone);
  VALUE ret = Data_Wrap_Struct(klass, tone_mark, -1, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_tone_initialize(int argc, VALUE *argv, VALUE self) {
  Tone *ptr = convertTone(self);
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
static VALUE rb_tone_initialize_copy(VALUE self, VALUE orig) {
  Tone *ptr = convertTone(self);
  Tone *orig_ptr = convertTone(orig);
  ptr->red = orig_ptr->red;
  ptr->green = orig_ptr->green;
  ptr->blue = orig_ptr->blue;
  ptr->gray = orig_ptr->gray;
  return Qnil;
}

static VALUE rb_tone_set(int argc, VALUE *argv, VALUE self) {
  Tone *ptr = convertTone(self);
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
      ptr->set(convertTone(argv[0]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1 or 3..4)", argc);
      break;
  }
  return Qnil;
}

static VALUE rb_tone_red(VALUE self) {
  Tone *ptr = convertTone(self);
  return DBL2NUM(ptr->red);
}
static VALUE rb_tone_set_red(VALUE self, VALUE red) {
  Tone *ptr = convertTone(self);
  ptr->red = NUM2DBL(red);
  return red;
}
static VALUE rb_tone_green(VALUE self) {
  Tone *ptr = convertTone(self);
  return DBL2NUM(ptr->green);
}
static VALUE rb_tone_set_green(VALUE self, VALUE green) {
  Tone *ptr = convertTone(self);
  ptr->green = NUM2DBL(green);
  return green;
}
static VALUE rb_tone_blue(VALUE self) {
  Tone *ptr = convertTone(self);
  return DBL2NUM(ptr->blue);
}
static VALUE rb_tone_set_blue(VALUE self, VALUE blue) {
  Tone *ptr = convertTone(self);
  ptr->blue = NUM2DBL(blue);
  return blue;
}
static VALUE rb_tone_gray(VALUE self) {
  Tone *ptr = convertTone(self);
  return DBL2NUM(ptr->gray);
}
static VALUE rb_tone_set_gray(VALUE self, VALUE gray) {
  Tone *ptr = convertTone(self);
  ptr->gray = NUM2DBL(gray);
  return gray;
}
static VALUE rb_tone_old_load(VALUE, VALUE str) {
  VALUE ret = tone_alloc(rb_cTone);
  Tone *ptr = convertTone(ret);
  char *s = StringValuePtr(str);
  if(!s) return ret;
  ptr->red = readDouble(s);
  ptr->green = readDouble(s+8);
  ptr->blue = readDouble(s+16);
  ptr->gray = readDouble(s+24);
  return ret;
}
static VALUE rb_tone_old_dump(VALUE self, VALUE) {
  Tone *ptr = convertTone(self);
  char s[32];
  writeDouble(s, ptr->red);
  writeDouble(s+8, ptr->green);
  writeDouble(s+16, ptr->blue);
  writeDouble(s+24, ptr->gray);
  VALUE ret = rb_str_new(s, 32);
  return ret;
}
