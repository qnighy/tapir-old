#include "Tone.h"
#include "misc.h"

struct Tone {
  double red, green, blue, gray;
};

static bool isTone(VALUE obj);
static struct Tone *convertTone(VALUE obj);
static void rb_tone_modify(VALUE obj);
static void tone_mark(struct Tone *);
static VALUE tone_alloc(VALUE klass);

VALUE rb_tone_new(double red, double green, double blue, double gray) {
  VALUE ret = tone_alloc(rb_cTone);
  struct Tone *ptr = convertTone(ret);
  ptr->red = saturateDouble(red, -255.0, 255.0);
  ptr->green = saturateDouble(green, -255.0, 255.0);
  ptr->blue = saturateDouble(blue, -255.0, 255.0);
  ptr->gray = saturateDouble(gray, 0.0, 255.0);
  return ret;
}
VALUE rb_tone_new2(void) {
  return rb_tone_new(0.0, 0.0, 0.0, 0.0);
}

bool rb_tone_equal(VALUE self, VALUE other) {
  struct Tone *ptr = convertTone(self);
  struct Tone *other_ptr = convertTone(other);
  return
    ptr->red == other_ptr->red &&
    ptr->green == other_ptr->green &&
    ptr->blue == other_ptr->blue &&
    ptr->gray == other_ptr->gray;
}

void rb_tone_set(
    VALUE self, double newred, double newgreen, double newblue,
    double newgray) {
  struct Tone *ptr = convertTone(self);
  rb_tone_modify(self);
  ptr->red = saturateDouble(newred, -255.0, 255.0);
  ptr->green = saturateDouble(newgreen, -255.0, 255.0);
  ptr->blue = saturateDouble(newblue, -255.0, 255.0);
  ptr->gray = saturateDouble(newgray, 0.0, 255.0);
}

void rb_tone_set2(VALUE self, VALUE other) {
  struct Tone *ptr = convertTone(self);
  struct Tone *other_ptr = convertTone(other);
  rb_tone_modify(self);
#ifdef CORRECT_RGSS_BEHAVIOR
  ptr->red = saturateDouble(other_ptr->red, -255.0, 255.0);
  ptr->green = saturateDouble(other_ptr->green, -255.0, 255.0);
  ptr->blue = saturateDouble(other_ptr->blue, -255.0, 255.0);
  ptr->gray = saturateDouble(other_ptr->gray, 0.0, 255.0);
#else
  ptr->red = other_ptr->red;
  ptr->green = other_ptr->green;
  ptr->blue = other_ptr->blue;
  ptr->gray = other_ptr->gray;
#endif
}

double rb_tone_red(VALUE self) {
  struct Tone *ptr = convertTone(self);
  return ptr->red;
}
void rb_tone_set_red(VALUE self, double newval) {
  struct Tone *ptr = convertTone(self);
  rb_tone_modify(self);
  ptr->red = saturateDouble(newval, -255.0, 255.0);
}
double rb_tone_green(VALUE self) {
  struct Tone *ptr = convertTone(self);
  return ptr->green;
}
void rb_tone_set_green(VALUE self, double newval) {
  struct Tone *ptr = convertTone(self);
  rb_tone_modify(self);
  ptr->green = saturateDouble(newval, -255.0, 255.0);
}
double rb_tone_blue(VALUE self) {
  struct Tone *ptr = convertTone(self);
  return ptr->blue;
}
void rb_tone_set_blue(VALUE self, double newval) {
  struct Tone *ptr = convertTone(self);
  rb_tone_modify(self);
  ptr->blue = saturateDouble(newval, -255.0, 255.0);
}
double rb_tone_gray(VALUE self) {
  struct Tone *ptr = convertTone(self);
  return ptr->gray;
}
void rb_tone_set_gray(VALUE self, double newval) {
  struct Tone *ptr = convertTone(self);
  rb_tone_modify(self);
  ptr->gray = saturateDouble(newval, 0.0, 255.0);
}

static VALUE rb_tone_m_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_tone_m_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_tone_m_equal(VALUE self, VALUE other);

static VALUE rb_tone_m_set(int argc, VALUE *argv, VALUE self);

static VALUE rb_tone_m_red(VALUE self);
static VALUE rb_tone_m_set_red(VALUE self, VALUE red);
static VALUE rb_tone_m_green(VALUE self);
static VALUE rb_tone_m_set_green(VALUE self, VALUE green);
static VALUE rb_tone_m_blue(VALUE self);
static VALUE rb_tone_m_set_blue(VALUE self, VALUE blue);
static VALUE rb_tone_m_gray(VALUE self);
static VALUE rb_tone_m_set_gray(VALUE self, VALUE gray);
static VALUE rb_tone_m_to_s(VALUE self);
static VALUE rb_tone_s_old_load(VALUE self, VALUE s);
static VALUE rb_tone_m_old_dump(VALUE self, VALUE lim);

VALUE rb_cTone;

/*
 * Tones contain red, green, blue and gray fields.
 *
 * Each field is a float between 0.0 and 255.0.
 */
void Init_Tone(void) {
  rb_cTone = rb_define_class("Tone", rb_cObject);
  rb_define_alloc_func(rb_cTone, tone_alloc);
  rb_define_private_method(rb_cTone, "initialize", rb_tone_m_initialize, -1);
  rb_define_private_method(rb_cTone, "initialize_copy",
      rb_tone_m_initialize_copy, 1);
  rb_define_method(rb_cTone, "==", rb_tone_m_equal, 1);
  rb_define_method(rb_cTone, "set", rb_tone_m_set, -1);
  rb_define_method(rb_cTone, "red", rb_tone_m_red, 0);
  rb_define_method(rb_cTone, "red=", rb_tone_m_set_red, 1);
  rb_define_method(rb_cTone, "green", rb_tone_m_green, 0);
  rb_define_method(rb_cTone, "green=", rb_tone_m_set_green, 1);
  rb_define_method(rb_cTone, "blue", rb_tone_m_blue, 0);
  rb_define_method(rb_cTone, "blue=", rb_tone_m_set_blue, 1);
  rb_define_method(rb_cTone, "gray", rb_tone_m_gray, 0);
  rb_define_method(rb_cTone, "gray=", rb_tone_m_set_gray, 1);
  rb_define_method(rb_cTone, "to_s", rb_tone_m_to_s, 0);
  rb_define_singleton_method(rb_cTone, "_load", rb_tone_s_old_load, 1);
  rb_define_method(rb_cTone, "_dump", rb_tone_m_old_dump, 1);
}

bool isTone(VALUE obj) {
  if(TYPE(obj) != T_DATA) return false;
  return RDATA(obj)->dmark == (void(*)(void*))tone_mark;
}

struct Tone *convertTone(VALUE obj) {
  Check_Type(obj, T_DATA);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(RDATA(obj)->dmark != (void(*)(void*))tone_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Tone",
        rb_class2name(rb_obj_class(obj)));
  }
#endif
  struct Tone *ret;
  Data_Get_Struct(obj, struct Tone, ret);
  return ret;
}

static void rb_tone_modify(VALUE obj) {
#ifdef CORRECT_RGSS_BEHAVIOR
  if(OBJ_FROZEN(obj)) rb_error_frozen("Tone");
  if(!OBJ_UNTRUSTED(obj) && rb_safe_level() >= 4) {
    rb_raise(rb_eSecurityError, "Insecure: can't modify Tone");
  }
#endif
}

static void tone_mark(struct Tone *ptr) {}

static VALUE tone_alloc(VALUE klass) {
  struct Tone *ptr = ALLOC(struct Tone);
  ptr->red = 0.0;
  ptr->green = 0.0;
  ptr->blue = 0.0;
  ptr->gray = 0.0;
  VALUE ret = Data_Wrap_Struct(klass, tone_mark, -1, ptr);
  return ret;
}

/*
 * call-seq:
 *   Tone.new(red, green, blue, gray=0.0)
 *   Tone.new
 *
 * Returns a new tone. In the second form, it initializes all fields by 0.0.
 */
static VALUE rb_tone_m_initialize(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      rb_tone_set(self, 0.0, 0.0, 0.0, 0.0);
      break;
    case 1:
    case 2:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (3 for %d)", argc);
      break;
    case 3:
      rb_tone_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          0.0);
      break;
    case 4:
      rb_tone_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          NUM2DBL(argv[3]));
      break;
    default:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
  }
  return Qnil;
}

static VALUE rb_tone_m_initialize_copy(VALUE self, VALUE orig) {
  rb_tone_set2(self, orig);
  return Qnil;
}

/*
 * call-seq:
 *    color == other -> bool
 *
 * Compares it to another tone.
 */
static VALUE rb_tone_m_equal(VALUE self, VALUE other) {
  if(!isTone(other)) return Qfalse;
  return rb_tone_equal(self, other) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *    tone.set(red, green, blue, gray=0.0) -> tone
 *    tone.set(other) -> tone
 *    tone.set -> tone
 *
 * Sets all fields. In the second form, it copies all fields from
 * <code>other</code>. In the third form, it initializes all fields by 0.0.
 *
 * It returns the tone itself.
 */
static VALUE rb_tone_m_set(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      rb_tone_set(self, 0.0, 0.0, 0.0, 0.0);
      break;
    case 1:
      rb_tone_set2(self, argv[0]);
      break;
    case 2:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (3 for %d)", argc);
      break;
    case 3:
      rb_tone_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          0.0);
      break;
    case 4:
      rb_tone_set(
          self,
          NUM2DBL(argv[0]),
          NUM2DBL(argv[1]),
          NUM2DBL(argv[2]),
          NUM2DBL(argv[3]));
      break;
    default:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
      break;
  }
  return self;
}

/*
 * call-seq:
 *   tone.red -> float
 *
 * Returns the red value of the tone.
 */
static VALUE rb_tone_m_red(VALUE self) {
  return DBL2NUM(rb_tone_red(self));
}

/*
 * call-seq:
 *   tone.red = newval -> newval
 *
 * Sets the red value of the tone.
 */
static VALUE rb_tone_m_set_red(VALUE self, VALUE newval) {
  rb_tone_set_red(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   tone.green -> float
 *
 * Returns the green value of the tone.
 */
static VALUE rb_tone_m_green(VALUE self) {
  return DBL2NUM(rb_tone_green(self));
}

/*
 * call-seq:
 *   tone.green = newval -> newval
 *
 * Sets the green value of the tone.
 */
static VALUE rb_tone_m_set_green(VALUE self, VALUE newval) {
  rb_tone_set_green(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   tone.blue -> float
 *
 * Returns the blue value of the tone.
 */
static VALUE rb_tone_m_blue(VALUE self) {
  return DBL2NUM(rb_tone_blue(self));
}

/*
 * call-seq:
 *   tone.blue = newval -> newval
 *
 * Sets the blue value of the tone.
 */
static VALUE rb_tone_m_set_blue(VALUE self, VALUE newval) {
  rb_tone_set_blue(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   tone.gray -> float
 *
 * Returns the gray value of the tone.
 */
static VALUE rb_tone_m_gray(VALUE self) {
  return DBL2NUM(rb_tone_gray(self));
}

/*
 * call-seq:
 *   tone.gray = newval -> newval
 *
 * Sets the gray value of the tone.
 */
static VALUE rb_tone_m_set_gray(VALUE self, VALUE newval) {
  rb_tone_set_gray(self, NUM2DBL(newval));
  return newval;
}

/*
 * call-seq:
 *   tone.to_s -> string
 *
 * Returns the string representation of the tone.
 */
static VALUE rb_tone_m_to_s(VALUE self) {
  struct Tone *ptr = convertTone(self);
  char s[100];
  snprintf(s, sizeof(s), "(%f, %f, %f, %f)",
      ptr->red, ptr->green, ptr->blue, ptr->gray);
  return rb_str_new2(s);
}

/*
 * call-seq:
 *   Tone._load(str) -> tone
 *
 * Loads a tone from <code>str</code>. Used in <code>Marshal.load</code>.
 */
static VALUE rb_tone_s_old_load(VALUE klass, VALUE str) {
  VALUE ret = tone_alloc(rb_cTone);
  struct Tone *ptr = convertTone(ret);
  StringValue(str);
#ifdef CORRECT_RGSS_BEHAVIOR
  Check_Type(str, T_STRING);
#endif
  const char *s = RSTRING_PTR(str);
  rb_tone_modify(ret);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(RSTRING_LEN(str) != sizeof(double)*4) {
    rb_raise(rb_eArgError, "Corrupted marshal data for Tone.");
  }
  ptr->red = saturateDouble(readDouble(s+sizeof(double)*0), -255.0, 255.0);
  ptr->green = saturateDouble(readDouble(s+sizeof(double)*1), -255.0, 255.0);
  ptr->blue = saturateDouble(readDouble(s+sizeof(double)*2), -255.0, 255.0);
  ptr->gray = saturateDouble(readDouble(s+sizeof(double)*3), 0.0, 255.0);
#else
  if(!s) return ret;
  ptr->red = readDouble(s+sizeof(double)*0);
  ptr->green = readDouble(s+sizeof(double)*1);
  ptr->blue = readDouble(s+sizeof(double)*2);
  ptr->gray = readDouble(s+sizeof(double)*3);
#endif
  return ret;
}

/*
 * call-seq:
 *   tone._dump(limit) -> string
 *
 * Dumps a tone to a string. Used in <code>Marshal.dump</code>.
 */
static VALUE rb_tone_m_old_dump(VALUE self, VALUE limit) {
  struct Tone *ptr = convertTone(self);
  char s[sizeof(double)*4];
  writeDouble(s+sizeof(double)*0, ptr->red);
  writeDouble(s+sizeof(double)*1, ptr->green);
  writeDouble(s+sizeof(double)*2, ptr->blue);
  writeDouble(s+sizeof(double)*3, ptr->gray);
  VALUE ret = rb_str_new(s, sizeof(s));
  return ret;
}
