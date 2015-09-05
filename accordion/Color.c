#include "Color.h"
#include "misc.h"

struct Color {
  double red, green, blue, alpha;
};

static bool isColor(VALUE obj);
static struct Color *convertColor(VALUE obj);
static void rb_color_modify(VALUE obj);
static void color_mark(struct Color *);
static VALUE color_alloc(VALUE klass);

VALUE rb_color_new(double red, double green, double blue, double alpha) {
  VALUE ret = color_alloc(rb_cColor);
  struct Color *ptr = convertColor(ret);
#ifdef CORRECT_RGSS_BEHAVIOR
  ptr->red = saturateDouble(red, 0.0, 255.0);
  ptr->green = saturateDouble(green, 0.0, 255.0);
  ptr->blue = saturateDouble(blue, 0.0, 255.0);
#else
  ptr->red = saturateDouble(red, -255.0, 255.0);
  ptr->green = saturateDouble(green, -255.0, 255.0);
  ptr->blue = saturateDouble(blue, -255.0, 255.0);
#endif
  ptr->alpha = saturateDouble(alpha, 0.0, 255.0);
  return ret;
}
VALUE rb_color_new2(void) {
  return rb_color_new(0.0, 0.0, 0.0, 0.0);
}

bool rb_color_equal(VALUE self, VALUE other) {
  struct Color *ptr = convertColor(self);
  struct Color *other_ptr = convertColor(other);
  return
    ptr->red == other_ptr->red &&
    ptr->green == other_ptr->green &&
    ptr->blue == other_ptr->blue &&
    ptr->alpha == other_ptr->alpha;
}

void rb_color_set(
    VALUE self, double newred, double newgreen, double newblue,
    double newalpha) {
  struct Color *ptr = convertColor(self);
  rb_color_modify(self);
#ifdef CORRECT_RGSS_BEHAVIOR
  ptr->red = saturateDouble(newred, 0.0, 255.0);
  ptr->green = saturateDouble(newgreen, 0.0, 255.0);
  ptr->blue = saturateDouble(newblue, 0.0, 255.0);
#else
  ptr->red = saturateDouble(newred, -255.0, 255.0);
  ptr->green = saturateDouble(newgreen, -255.0, 255.0);
  ptr->blue = saturateDouble(newblue, -255.0, 255.0);
#endif
  ptr->alpha = saturateDouble(newalpha, 0.0, 255.0);
}

void rb_color_set2(VALUE self, VALUE other) {
  struct Color *ptr = convertColor(self);
  struct Color *other_ptr = convertColor(other);
  rb_color_modify(self);
#ifdef CORRECT_RGSS_BEHAVIOR
  ptr->red = saturateDouble(other_ptr->red, 0.0, 255.0);
  ptr->green = saturateDouble(other_ptr->green, 0.0, 255.0);
  ptr->blue = saturateDouble(other_ptr->blue, 0.0, 255.0);
  ptr->alpha = saturateDouble(other_ptr->alpha, 0.0, 255.0);
#else
  ptr->red = other_ptr->red;
  ptr->green = other_ptr->green;
  ptr->blue = other_ptr->blue;
  ptr->alpha = other_ptr->alpha;
#endif
}

double rb_color_red(VALUE self) {
  struct Color *ptr = convertColor(self);
  return ptr->red;
}
void rb_color_set_red(VALUE self, double newval) {
  struct Color *ptr = convertColor(self);
  rb_color_modify(self);
#ifdef CORRECT_RGSS_BEHAVIOR
  ptr->red = saturateDouble(newval, 0.0, 255.0);
#else
  ptr->red = saturateDouble(newval, -255.0, 255.0);
#endif
}
double rb_color_green(VALUE self) {
  struct Color *ptr = convertColor(self);
  return ptr->green;
}
void rb_color_set_green(VALUE self, double newval) {
  struct Color *ptr = convertColor(self);
  rb_color_modify(self);
#ifdef CORRECT_RGSS_BEHAVIOR
  ptr->green = saturateDouble(newval, 0.0, 255.0);
#else
  ptr->green = saturateDouble(newval, -255.0, 255.0);
#endif
}
double rb_color_blue(VALUE self) {
  struct Color *ptr = convertColor(self);
  return ptr->blue;
}
void rb_color_set_blue(VALUE self, double newval) {
  struct Color *ptr = convertColor(self);
  rb_color_modify(self);
#ifdef CORRECT_RGSS_BEHAVIOR
  ptr->blue = saturateDouble(newval, 0.0, 255.0);
#else
  ptr->blue = saturateDouble(newval, -255.0, 255.0);
#endif
}
double rb_color_alpha(VALUE self) {
  struct Color *ptr = convertColor(self);
  return ptr->alpha;
}
void rb_color_set_alpha(VALUE self, double newval) {
  struct Color *ptr = convertColor(self);
  rb_color_modify(self);
  ptr->alpha = saturateDouble(newval, 0.0, 255.0);
}

static VALUE rb_color_m_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_color_m_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_color_m_equal(VALUE self, VALUE other);

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
static VALUE rb_color_s_old_load(VALUE self, VALUE s);
static VALUE rb_color_m_old_dump(VALUE self, VALUE lim);

VALUE rb_cColor;

/*
 * Colors contain red, green, blue and alpha fields.
 *
 * Each field is a float between 0.0 and 255.0.
 */
void Init_Color(void) {
  rb_cColor = rb_define_class("Color", rb_cObject);
  rb_define_alloc_func(rb_cColor, color_alloc);
  rb_define_private_method(rb_cColor, "initialize", rb_color_m_initialize, -1);
  rb_define_private_method(rb_cColor, "initialize_copy",
      rb_color_m_initialize_copy, 1);
  rb_define_method(rb_cColor, "==", rb_color_m_equal, 1);
  rb_define_method(rb_cColor, "set", rb_color_m_set, -1);
  rb_define_method(rb_cColor, "red", rb_color_m_red, 0);
  rb_define_method(rb_cColor, "red=", rb_color_m_set_red, 1);
  rb_define_method(rb_cColor, "green", rb_color_m_green, 0);
  rb_define_method(rb_cColor, "green=", rb_color_m_set_green, 1);
  rb_define_method(rb_cColor, "blue", rb_color_m_blue, 0);
  rb_define_method(rb_cColor, "blue=", rb_color_m_set_blue, 1);
  rb_define_method(rb_cColor, "alpha", rb_color_m_alpha, 0);
  rb_define_method(rb_cColor, "alpha=", rb_color_m_set_alpha, 1);
  rb_define_method(rb_cColor, "to_s", rb_color_m_to_s, 0);
  rb_define_singleton_method(rb_cColor, "_load", rb_color_s_old_load, 1);
  rb_define_method(rb_cColor, "_dump", rb_color_m_old_dump, 1);
}

bool isColor(VALUE obj) {
  if(TYPE(obj) != T_DATA) return false;
  return RDATA(obj)->dmark == (void(*)(void*))color_mark;
}

struct Color *convertColor(VALUE obj) {
  Check_Type(obj, T_DATA);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(RDATA(obj)->dmark != (void(*)(void*))color_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Color",
        rb_class2name(rb_obj_class(obj)));
  }
#endif
  struct Color *ret;
  Data_Get_Struct(obj, struct Color, ret);
  return ret;
}

static void rb_color_modify(VALUE obj) {
#ifdef CORRECT_RGSS_BEHAVIOR
  if(OBJ_FROZEN(obj)) rb_error_frozen("Color");
  if(!OBJ_UNTRUSTED(obj) && rb_safe_level() >= 4) {
    rb_raise(rb_eSecurityError, "Insecure: can't modify Color");
  }
#endif
}

static void color_mark(struct Color *ptr) {}

static VALUE color_alloc(VALUE klass) {
  struct Color *ptr = ALLOC(struct Color);
  ptr->red = 0.0;
  ptr->green = 0.0;
  ptr->blue = 0.0;
  ptr->alpha = 0.0;
  VALUE ret = Data_Wrap_Struct(klass, color_mark, -1, ptr);
  return ret;
}

/*
 * call-seq:
 *   Color.new(red, green, blue, alpha=255.0)
 *   Color.new
 *
 * Returns a new color. In the second form, it initializes all fields by 0.0.
 */
static VALUE rb_color_m_initialize(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      rb_color_set(self, 0.0, 0.0, 0.0, 0.0);
      break;
    case 1:
    case 2:
#ifdef CORRECT_RGSS_BEHAVIOR
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0 or 3..4)", argc);
#else
      rb_raise(rb_eArgError,
          "wrong number of arguments (3 for %d)", argc);
#endif
      break;
    case 3:
      rb_color_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          255.0);
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
#ifdef CORRECT_RGSS_BEHAVIOR
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0 or 3..4)", argc);
#else
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
#endif
      break;
  }
  return Qnil;
}

static VALUE rb_color_m_initialize_copy(VALUE self, VALUE orig) {
  rb_color_set2(self, orig);
  return Qnil;
}

/*
 * call-seq:
 *    color == other -> bool
 *
 * Compares it to another color.
 */
static VALUE rb_color_m_equal(VALUE self, VALUE other) {
  if(!isColor(other)) return Qfalse;
  return rb_color_equal(self, other) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *    color.set(red, green, blue, alpha=255.0) -> color
 *    color.set(other) -> color
 *    color.set -> color
 *
 * Sets all fields. In the second form, it copies all fields from
 * <code>other</code>. In the third form, it initializes all fields by 0.0.
 *
 * It returns the color itself.
 */
static VALUE rb_color_m_set(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
#ifdef CORRECT_RGSS_BEHAVIOR
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1, 3..4)", argc);
#else
      rb_color_set(self, 0.0, 0.0, 0.0, 0.0);
#endif
      break;
    case 1:
      rb_color_set2(self, argv[0]);
      break;
    case 2:
#ifdef CORRECT_RGSS_BEHAVIOR
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1, 3..4)", argc);
#else
      rb_raise(rb_eArgError,
          "wrong number of arguments (3 for %d)", argc);
#endif
      break;
    case 3:
      rb_color_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          255.0);
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
#ifdef CORRECT_RGSS_BEHAVIOR
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1, 3..4)", argc);
#else
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
#endif
      break;
  }
  return self;
}

/*
 * call-seq:
 *   color.red -> float
 *
 * Returns the red value of the color.
 */
static VALUE rb_color_m_red(VALUE self) {
  return DBL2NUM(rb_color_red(self));
}

/*
 * call-seq:
 *   color.red = newval -> newval
 *
 * Sets the red value of the color.
 */
static VALUE rb_color_m_set_red(VALUE self, VALUE newval) {
  rb_color_set_red(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   color.green -> float
 *
 * Returns the green value of the color.
 */
static VALUE rb_color_m_green(VALUE self) {
  return DBL2NUM(rb_color_green(self));
}

/*
 * call-seq:
 *   color.green = newval -> newval
 *
 * Sets the green value of the color.
 */
static VALUE rb_color_m_set_green(VALUE self, VALUE newval) {
  rb_color_set_green(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   color.blue -> float
 *
 * Returns the blue value of the color.
 */
static VALUE rb_color_m_blue(VALUE self) {
  return DBL2NUM(rb_color_blue(self));
}

/*
 * call-seq:
 *   color.blue = newval -> newval
 *
 * Sets the blue value of the color.
 */
static VALUE rb_color_m_set_blue(VALUE self, VALUE newval) {
  rb_color_set_blue(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   color.alpha -> float
 *
 * Returns the alpha value of the color.
 */
static VALUE rb_color_m_alpha(VALUE self) {
  return DBL2NUM(rb_color_alpha(self));
}

/*
 * call-seq:
 *   color.alpha = newval -> newval
 *
 * Sets the alpha value of the color.
 */
static VALUE rb_color_m_set_alpha(VALUE self, VALUE newval) {
  rb_color_set_alpha(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   color.to_s -> string
 *
 * Returns the string representation of the color.
 */
static VALUE rb_color_m_to_s(VALUE self) {
  struct Color *ptr = convertColor(self);
  char s[100];
  snprintf(s, sizeof(s), "(%f, %f, %f, %f)",
      ptr->red, ptr->green, ptr->blue, ptr->alpha);
  return rb_str_new2(s);
}

/*
 * call-seq:
 *   Color._load(str) -> color
 *
 * Loads a color from <code>str</code>. Used in <code>Marshal.load</code>.
 */
static VALUE rb_color_s_old_load(VALUE klass, VALUE str) {
  VALUE ret = color_alloc(rb_cColor);
  struct Color *ptr = convertColor(ret);
  StringValue(str);
#ifdef CORRECT_RGSS_BEHAVIOR
  Check_Type(str, T_STRING);
#endif
  const char *s = RSTRING_PTR(str);
  rb_color_modify(ret);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(RSTRING_LEN(str) != sizeof(double)*4) {
    rb_raise(rb_eArgError, "Corrupted marshal data for Color.");
  }
  ptr->red = saturateDouble(readDouble(s+sizeof(double)*0), 0.0, 255.0);
  ptr->green = saturateDouble(readDouble(s+sizeof(double)*1), 0.0, 255.0);
  ptr->blue = saturateDouble(readDouble(s+sizeof(double)*2), 0.0, 255.0);
  ptr->alpha = saturateDouble(readDouble(s+sizeof(double)*3), 0.0, 255.0);
#else
  if(!s) return ret;
  ptr->red = readDouble(s+sizeof(double)*0);
  ptr->green = readDouble(s+sizeof(double)*1);
  ptr->blue = readDouble(s+sizeof(double)*2);
  ptr->alpha = readDouble(s+sizeof(double)*3);
#endif
  return ret;
}

/*
 * call-seq:
 *   color._dump(limit) -> string
 *
 * Dumps a color to a string. Used in <code>Marshal.dump</code>.
 */
static VALUE rb_color_m_old_dump(VALUE self, VALUE limit) {
  struct Color *ptr = convertColor(self);
  char s[sizeof(double)*4];
  writeDouble(s+sizeof(double)*0, ptr->red);
  writeDouble(s+sizeof(double)*1, ptr->green);
  writeDouble(s+sizeof(double)*2, ptr->blue);
  writeDouble(s+sizeof(double)*3, ptr->alpha);
  VALUE ret = rb_str_new(s, sizeof(s));
  return ret;
}
