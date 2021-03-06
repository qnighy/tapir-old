#include "Rect.h"
#include "misc.h"

struct Rect {
  int32_t x, y, width, height;
};

bool isRect(VALUE obj);
struct Rect *convertRect(RectVALUE obj);
static void rb_rect_modify(RectVALUE obj);
static void rect_mark(struct Rect *);
static RectVALUE rect_alloc(VALUE klass);

RectVALUE rb_rect_new(int32_t x, int32_t y, int32_t width, int32_t height) {
  RectVALUE ret = rect_alloc(rb_cRect);
  struct Rect *ptr = convertRect(ret);
  ptr->x = x;
  ptr->y = y;
  ptr->width = width;
  ptr->height = height;
  return ret;
}

RectVALUE rb_rect_new2(void) {
  return rb_rect_new(0, 0, 0, 0);
}

bool rb_rect_equal(RectVALUE self, VALUE other) {
  struct Rect *ptr = convertRect(self);
  struct Rect *other_ptr = convertRect(other);
  return
    ptr->x == other_ptr->x &&
    ptr->y == other_ptr->y &&
    ptr->width == other_ptr->width &&
    ptr->height == other_ptr->height;
}

void rb_rect_set(
    RectVALUE self, int32_t newx, int32_t newy,
    int32_t newwidth, int32_t newheight) {
  struct Rect *ptr = convertRect(self);
  rb_rect_modify(self);
  ptr->x = newx;
  ptr->y = newy;
  ptr->width = newwidth;
  ptr->height = newheight;
}

void rb_rect_set2(RectVALUE self, RectVALUE other) {
  struct Rect *ptr = convertRect(self);
  struct Rect *other_ptr = convertRect(other);
  rb_rect_modify(self);
  ptr->x = other_ptr->x;
  ptr->y = other_ptr->y;
  ptr->width = other_ptr->width;
  ptr->height = other_ptr->height;
}

int32_t rb_rect_x(RectVALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->x;
}
void rb_rect_set_x(RectVALUE self, int32_t newval) {
  struct Rect *ptr = convertRect(self);
  rb_rect_modify(self);
  ptr->x = newval;
}
int32_t rb_rect_y(RectVALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->y;
}
void rb_rect_set_y(RectVALUE self, int32_t newval) {
  struct Rect *ptr = convertRect(self);
  rb_rect_modify(self);
  ptr->y = newval;
}
int32_t rb_rect_width(RectVALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->width;
}
void rb_rect_set_width(RectVALUE self, int32_t newval) {
  struct Rect *ptr = convertRect(self);
  rb_rect_modify(self);
  ptr->width = newval;
}
int32_t rb_rect_height(RectVALUE self) {
  struct Rect *ptr = convertRect(self);
  return ptr->height;
}
void rb_rect_set_height(RectVALUE self, int32_t newval) {
  struct Rect *ptr = convertRect(self);
  rb_rect_modify(self);
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
static VALUE rb_rect_s_old_load(VALUE self, VALUE s);
static VALUE rb_rect_m_old_dump(VALUE self, VALUE lim);

VALUE rb_cRect;

/*
 * Rectangles contain x, y, width and height fields.
 *
 * Each field is a signed 32-bit integer.
 */
void Init_Rect() {
  rb_cRect = rb_define_class("Rect", rb_cObject);
  rb_define_alloc_func(rb_cRect, rect_alloc);
  rb_define_private_method(rb_cRect, "initialize", rb_rect_m_initialize, -1);
  rb_define_private_method(rb_cRect, "initialize_copy",
      rb_rect_m_initialize_copy, 1);
  rb_define_method(rb_cRect, "==", rb_rect_m_equal, 1);
  rb_define_method(rb_cRect, "set", rb_rect_m_set, -1);
  rb_define_method(rb_cRect, "empty", rb_rect_m_empty, 0);
  rb_define_method(rb_cRect, "x", rb_rect_m_x, 0);
  rb_define_method(rb_cRect, "x=", rb_rect_m_set_x, 1);
  rb_define_method(rb_cRect, "y", rb_rect_m_y, 0);
  rb_define_method(rb_cRect, "y=", rb_rect_m_set_y, 1);
  rb_define_method(rb_cRect, "width", rb_rect_m_width, 0);
  rb_define_method(rb_cRect, "width=", rb_rect_m_set_width, 1);
  rb_define_method(rb_cRect, "height", rb_rect_m_height, 0);
  rb_define_method(rb_cRect, "height=", rb_rect_m_set_height, 1);
  rb_define_method(rb_cRect, "to_s", rb_rect_m_to_s, 0);
  rb_define_singleton_method(rb_cRect, "_load", rb_rect_s_old_load, 1);
  rb_define_method(rb_cRect, "_dump", rb_rect_m_old_dump, 1);
}

bool isRect(VALUE obj) {
  if(TYPE(obj) != T_DATA) return false;
  return RDATA(obj)->dmark == (void(*)(void*))rect_mark;
}

struct Rect *convertRect(RectVALUE obj) {
  Check_Type(obj, T_DATA);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(RDATA(obj)->dmark != (void(*)(void*))rect_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Rect",
        rb_class2name(rb_obj_class(obj)));
  }
#endif
  struct Rect *ret;
  Data_Get_Struct(obj, struct Rect, ret);
  return ret;
}

static void rb_rect_modify(RectVALUE obj) {
#ifdef CORRECT_RGSS_BEHAVIOR
  if(OBJ_FROZEN(obj)) rb_error_frozen("Rect");
  if(!OBJ_UNTRUSTED(obj) && rb_safe_level() >= 4) {
    rb_raise(rb_eSecurityError, "Insecure: can't modify Rect");
  }
#endif
}

static void rect_mark(struct Rect *ptr) {}

static RectVALUE rect_alloc(VALUE klass) {
  struct Rect *ptr = ALLOC(struct Rect);
  ptr->x = 0;
  ptr->y = 0;
  ptr->width = 0;
  ptr->height = 0;
  RectVALUE ret = Data_Wrap_Struct(klass, rect_mark, -1, ptr);
  return ret;
}

/*
 * call-seq:
 *   Rect.new(x, y, width, height)
 *   Rect.new
 *
 * Returns a new rectangle. In the second form, it initializes all fields by 0.
 */
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

/*
 * call-seq:
 *    rectangle == other -> bool
 *
 * Compares it to another rectangle.
 */
static VALUE rb_rect_m_equal(VALUE self, VALUE other) {
  if(!isRect(other)) return Qfalse;
  return rb_rect_equal(self, other) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *    rectangle.set(x, y, width, height) -> rectangle
 *    rectangle.set(other) -> rectangle
 *
 * Sets all fields. In the second form, it copies all fields from
 * <code>other</code>.
 *
 * It returns the rectangle itself.
 */
static VALUE rb_rect_m_set(int argc, VALUE *argv, VALUE self) {
  switch(argc) {
    case 0:
      // RGSS BUG: It produces wrong messages.
      rb_raise(rb_eArgError,
          "wrong number of arguments (4 for %d)", argc);
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

/*
 * call-seq:
 *    rectangle.empty -> rectangle
 *
 * Initializes all fields by 0.
 *
 * It returns the rectangle itself.
 */
static VALUE rb_rect_m_empty(VALUE self) {
  rb_rect_set(self, 0, 0, 0, 0);
  return self;
}

/*
 * call-seq:
 *    rectangle.x -> integer
 *
 * Returns the x value of the rectangle.
 */
static VALUE rb_rect_m_x(VALUE self) {
  return INT2NUM(rb_rect_x(self));
}

/*
 * call-seq:
 *    rectangle.x = newval -> newval
 *
 * Sets the x value of the rectangle.
 */
static VALUE rb_rect_m_set_x(VALUE self, VALUE newval) {
  rb_rect_set_x(self, NUM2INT(newval));
  return newval;
}

/*
 * call-seq:
 *    rectangle.y -> integer
 *
 * Returns the y value of the rectangle.
 */
static VALUE rb_rect_m_y(VALUE self) {
  return INT2NUM(rb_rect_y(self));
}

/*
 * call-seq:
 *    rectangle.y = newval -> newval
 *
 * Sets the y value of the rectangle.
 */
static VALUE rb_rect_m_set_y(VALUE self, VALUE newval) {
  rb_rect_set_y(self, NUM2INT(newval));
  return newval;
}

/*
 * call-seq:
 *    rectangle.width -> integer
 *
 * Returns the width value of the rectangle.
 */
static VALUE rb_rect_m_width(VALUE self) {
  return INT2NUM(rb_rect_width(self));
}

/*
 * call-seq:
 *    rectangle.width = newval -> newval
 *
 * Sets the width value of the rectangle.
 */
static VALUE rb_rect_m_set_width(VALUE self, VALUE newval) {
  rb_rect_set_width(self, NUM2INT(newval));
  return newval;
}

/*
 * call-seq:
 *    rectangle.height -> integer
 *
 * Returns the height value of the rectangle.
 */
static VALUE rb_rect_m_height(VALUE self) {
  return INT2NUM(rb_rect_height(self));
}

/*
 * call-seq:
 *    rectangle.height = newval -> newval
 *
 * Sets the height value of the rectangle.
 */
static VALUE rb_rect_m_set_height(VALUE self, VALUE newval) {
  rb_rect_set_height(self, NUM2INT(newval));
  return newval;
}

/*
 * call-seq:
 *    rectangle.to_s -> string
 *
 * Returns the string representation of the rectangle.
 */
static VALUE rb_rect_m_to_s(VALUE self) {
  struct Rect *ptr = convertRect(self);
  char s[50];
  snprintf(s, sizeof(s), "(%d, %d, %d, %d)",
      ptr->x, ptr->y, ptr->width, ptr->height);
  return rb_str_new2(s);
}

/*
 * call-seq:
 *   Rect._load(str) -> rectangle
 *
 * Loads a rectangle from <code>str</code>. Used in <code>Marshal.load</code>.
 */
static VALUE rb_rect_s_old_load(VALUE klass, VALUE str) {
  VALUE ret = rect_alloc(rb_cRect);
  struct Rect *ptr = convertRect(ret);
  StringValue(str);
#ifdef CORRECT_RGSS_BEHAVIOR
  Check_Type(str, T_STRING);
#endif
  const char *s = RSTRING_PTR(str);
  rb_rect_modify(ret);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(RSTRING_LEN(str) != sizeof(int32_t)*4) {
    rb_raise(rb_eArgError, "Corrupted marshal data for Rect.");
  }
#else
  if(!s) return ret;
#endif
  ptr->x = readInt32(s+sizeof(int32_t)*0);
  ptr->y = readInt32(s+sizeof(int32_t)*1);
  ptr->width = readInt32(s+sizeof(int32_t)*2);
  ptr->height = readInt32(s+sizeof(int32_t)*3);
  return ret;
}

/*
 * call-seq:
 *   rectangle._dump(limit) -> string
 *
 * Dumps a rectangle to a string. Used in <code>Marshal.dump</code>.
 */
static VALUE rb_rect_m_old_dump(VALUE self, VALUE limit) {
  struct Rect *ptr = convertRect(self);
  char s[sizeof(int32_t)*4];
  writeInt32(s+sizeof(int32_t)*0, ptr->x);
  writeInt32(s+sizeof(int32_t)*1, ptr->y);
  writeInt32(s+sizeof(int32_t)*2, ptr->width);
  writeInt32(s+sizeof(int32_t)*3, ptr->height);
  VALUE ret = rb_str_new(s, sizeof(s));
  return ret;
}
