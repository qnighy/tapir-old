#include "Font.h"
#include "misc.h"

void Font::initialize(VALUE name, int size) {
  this->name = name;
  this->size = size;
  this->bold = default_bold;
  this->italic = default_italic;
  this->shadow = default_shadow;
  this->outline = default_outline;
  this->color = Color::create();
  this->out_color = Color::create();
  this->color->set(default_color);
  this->out_color->set(default_out_color);
}
void Font::set(Font *font) {
  this->name = font->name;
  this->size = font->size;
  this->bold = font->bold;
  this->italic = font->italic;
  this->shadow = font->shadow;
  this->outline = font->outline;
  this->color->set(font->color);
  this->out_color->set(font->out_color);
}

bool Font::exist(VALUE) {
  return true;
}

VALUE Font::default_name;
int Font::default_size;
bool Font::default_bold, Font::default_italic;
bool Font::default_shadow, Font::default_outline;
Color *Font::default_color, *Font::default_out_color;

static void font_mark(Font *);
static VALUE font_alloc(VALUE klass);

static VALUE rb_font_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_font_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_font_exist(VALUE self, VALUE name);

static VALUE rb_font_name(VALUE self);
static VALUE rb_font_set_name(VALUE self, VALUE name);
static VALUE rb_font_size(VALUE self);
static VALUE rb_font_set_size(VALUE self, VALUE size);
static VALUE rb_font_bold(VALUE self);
static VALUE rb_font_set_bold(VALUE self, VALUE bold);
static VALUE rb_font_italic(VALUE self);
static VALUE rb_font_set_italic(VALUE self, VALUE italic);
static VALUE rb_font_shadow(VALUE self);
static VALUE rb_font_set_shadow(VALUE self, VALUE shadow);
static VALUE rb_font_outline(VALUE self);
static VALUE rb_font_set_outline(VALUE self, VALUE outline);
static VALUE rb_font_color(VALUE self);
static VALUE rb_font_set_color(VALUE self, VALUE color);
static VALUE rb_font_out_color(VALUE self);
static VALUE rb_font_set_out_color(VALUE self, VALUE out_color);

static VALUE rb_font_default_name(VALUE self);
static VALUE rb_font_set_default_name(VALUE self, VALUE default_name);
static VALUE rb_font_default_size(VALUE self);
static VALUE rb_font_set_default_size(VALUE self, VALUE default_size);
static VALUE rb_font_default_bold(VALUE self);
static VALUE rb_font_set_default_bold(VALUE self, VALUE default_bold);
static VALUE rb_font_default_italic(VALUE self);
static VALUE rb_font_set_default_italic(VALUE self, VALUE default_italic);
static VALUE rb_font_default_shadow(VALUE self);
static VALUE rb_font_set_default_shadow(VALUE self, VALUE default_shadow);
static VALUE rb_font_default_outline(VALUE self);
static VALUE rb_font_set_default_outline(VALUE self, VALUE default_outline);
static VALUE rb_font_default_color(VALUE self);
static VALUE rb_font_set_default_color(VALUE self, VALUE default_color);
static VALUE rb_font_default_out_color(VALUE self);
static VALUE rb_font_set_default_out_color(VALUE self, VALUE default_out_color);

VALUE rb_cFont;
void InitFont() {
  rb_cFont = rb_define_class("Font", rb_cObject);
  rb_define_alloc_func(rb_cFont, font_alloc);
  rb_define_method(rb_cFont, "initialize",
      (VALUE(*)(...))rb_font_initialize, -1);
  rb_define_method(rb_cFont, "initialize_copy",
      (VALUE(*)(...))rb_font_initialize_copy, 1);

  rb_define_singleton_method(rb_cFont, "exist?",
      (VALUE(*)(...))rb_font_exist, 1);

  rb_define_method(rb_cFont, "name",
      (VALUE(*)(...))rb_font_name, 0);
  rb_define_method(rb_cFont, "name=",
      (VALUE(*)(...))rb_font_set_name, 1);
  rb_define_method(rb_cFont, "size",
      (VALUE(*)(...))rb_font_size, 0);
  rb_define_method(rb_cFont, "size=",
      (VALUE(*)(...))rb_font_set_size, 1);
  rb_define_method(rb_cFont, "bold",
      (VALUE(*)(...))rb_font_bold, 0);
  rb_define_method(rb_cFont, "bold=",
      (VALUE(*)(...))rb_font_set_bold, 1);
  rb_define_method(rb_cFont, "italic",
      (VALUE(*)(...))rb_font_italic, 0);
  rb_define_method(rb_cFont, "italic=",
      (VALUE(*)(...))rb_font_set_italic, 1);
  rb_define_method(rb_cFont, "shadow",
      (VALUE(*)(...))rb_font_shadow, 0);
  rb_define_method(rb_cFont, "shadow=",
      (VALUE(*)(...))rb_font_set_shadow, 1);
  rb_define_method(rb_cFont, "outline",
      (VALUE(*)(...))rb_font_outline, 0);
  rb_define_method(rb_cFont, "outline=",
      (VALUE(*)(...))rb_font_set_outline, 1);
  rb_define_method(rb_cFont, "color",
      (VALUE(*)(...))rb_font_color, 0);
  rb_define_method(rb_cFont, "color=",
      (VALUE(*)(...))rb_font_set_color, 1);
  rb_define_method(rb_cFont, "out_color",
      (VALUE(*)(...))rb_font_out_color, 0);
  rb_define_method(rb_cFont, "out_color=",
      (VALUE(*)(...))rb_font_set_out_color, 1);

  rb_define_singleton_method(rb_cFont, "default_name",
      (VALUE(*)(...))rb_font_default_name, 0);
  rb_define_singleton_method(rb_cFont, "default_name=",
      (VALUE(*)(...))rb_font_set_default_name, 1);
  rb_define_singleton_method(rb_cFont, "default_size",
      (VALUE(*)(...))rb_font_default_size, 0);
  rb_define_singleton_method(rb_cFont, "default_size=",
      (VALUE(*)(...))rb_font_set_default_size, 1);
  rb_define_singleton_method(rb_cFont, "default_bold",
      (VALUE(*)(...))rb_font_default_bold, 0);
  rb_define_singleton_method(rb_cFont, "default_bold=",
      (VALUE(*)(...))rb_font_set_default_bold, 1);
  rb_define_singleton_method(rb_cFont, "default_italic",
      (VALUE(*)(...))rb_font_default_italic, 0);
  rb_define_singleton_method(rb_cFont, "default_italic=",
      (VALUE(*)(...))rb_font_set_default_italic, 1);
  rb_define_singleton_method(rb_cFont, "default_shadow",
      (VALUE(*)(...))rb_font_default_shadow, 0);
  rb_define_singleton_method(rb_cFont, "default_shadow=",
      (VALUE(*)(...))rb_font_set_default_shadow, 1);
  rb_define_singleton_method(rb_cFont, "default_outline",
      (VALUE(*)(...))rb_font_default_outline, 0);
  rb_define_singleton_method(rb_cFont, "default_outline=",
      (VALUE(*)(...))rb_font_set_default_outline, 1);
  rb_define_singleton_method(rb_cFont, "default_color",
      (VALUE(*)(...))rb_font_default_color, 0);
  rb_define_singleton_method(rb_cFont, "default_color=",
      (VALUE(*)(...))rb_font_set_default_color, 1);
  rb_define_singleton_method(rb_cFont, "default_out_color",
      (VALUE(*)(...))rb_font_default_out_color, 0);
  rb_define_singleton_method(rb_cFont, "default_out_color=",
      (VALUE(*)(...))rb_font_set_default_out_color, 1);

  Font::default_name = rb_str_new2("VL Gothic");
  rb_gc_register_address(&Font::default_name);
  Font::default_bold = false;
  Font::default_italic = false;
  Font::default_shadow = false;
  Font::default_outline = true;
  Font::default_color = Color::create();
  Font::default_out_color = Color::create();
  rb_gc_register_address(&Font::default_color->rb_parent);
  rb_gc_register_address(&Font::default_out_color->rb_parent);
}

Font *convertFont(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))font_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Font",
        rb_class2name(rb_obj_class(obj)));
  }
  Font *ret;
  Data_Get_Struct(obj, Font, ret);
  return ret;
}
Font *convertFontOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertFont(obj);
}

Font *Font::create(VALUE name, int size) {
  VALUE ret = font_alloc(rb_cFont);
  Font *ptr = convertFont(ret);
  ptr->initialize(name, size);
  return ptr;
}

static void font_mark(Font *ptr) {
  rb_gc_mark(ptr->name);
  rb_gc_mark(ptr->color->rb_parent);
  rb_gc_mark(ptr->out_color->rb_parent);
}

static VALUE font_alloc(VALUE klass) {
  Font *ptr = ALLOC(Font);
  VALUE ret = Data_Wrap_Struct(klass, font_mark, -1, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_font_initialize(int argc, VALUE *argv, VALUE self) {
  Font *ptr = convertFont(self);
  switch(argc) {
    case 2:
      ptr->initialize(argv[0], NUM2INT(argv[1]));
      break;
    case 1:
      ptr->initialize(argv[0]);
      break;
    case 0:
      ptr->initialize();
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0..2)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_font_initialize_copy(VALUE self, VALUE orig) {
  Font *ptr = convertFont(self);
  Font *orig_ptr = convertFont(orig);
  ptr->name = orig_ptr->name;
  ptr->size = orig_ptr->size;
  ptr->bold = orig_ptr->bold;
  ptr->italic = orig_ptr->italic;
  ptr->shadow = orig_ptr->shadow;
  ptr->outline = orig_ptr->outline;
  ptr->color->set(orig_ptr->color);
  ptr->out_color->set(orig_ptr->out_color);
  return Qnil;
}

static VALUE rb_font_exist(VALUE self, VALUE name) {
  Font *ptr = convertFont(self);
  return ptr->exist(name) ? Qtrue : Qfalse;
}

static VALUE rb_font_name(VALUE self) {
  Font *ptr = convertFont(self);
  return ptr->name;
}
static VALUE rb_font_set_name(VALUE self, VALUE name) {
  Font *ptr = convertFont(self);
  ptr->name = name;
  return name;
}
static VALUE rb_font_size(VALUE self) {
  Font *ptr = convertFont(self);
  return INT2NUM(ptr->size);
}
static VALUE rb_font_set_size(VALUE self, VALUE size) {
  Font *ptr = convertFont(self);
  ptr->size = NUM2INT(size);
  return size;
}
static VALUE rb_font_bold(VALUE self) {
  Font *ptr = convertFont(self);
  return (ptr->bold) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_bold(VALUE self, VALUE bold) {
  Font *ptr = convertFont(self);
  ptr->bold = RTEST(bold);
  return bold;
}
static VALUE rb_font_italic(VALUE self) {
  Font *ptr = convertFont(self);
  return (ptr->italic) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_italic(VALUE self, VALUE italic) {
  Font *ptr = convertFont(self);
  ptr->italic = RTEST(italic);
  return italic;
}
static VALUE rb_font_shadow(VALUE self) {
  Font *ptr = convertFont(self);
  return (ptr->shadow) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_shadow(VALUE self, VALUE shadow) {
  Font *ptr = convertFont(self);
  ptr->shadow = RTEST(shadow);
  return shadow;
}
static VALUE rb_font_outline(VALUE self) {
  Font *ptr = convertFont(self);
  return (ptr->outline) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_outline(VALUE self, VALUE outline) {
  Font *ptr = convertFont(self);
  ptr->outline = RTEST(outline);
  return outline;
}
static VALUE rb_font_color(VALUE self) {
  Font *ptr = convertFont(self);
  return (ptr->color)->rb_parent;
}
static VALUE rb_font_set_color(VALUE self, VALUE color) {
  Font *ptr = convertFont(self);
  ptr->color->set(convertColor(color));
  return color;
}
static VALUE rb_font_out_color(VALUE self) {
  Font *ptr = convertFont(self);
  return (ptr->out_color)->rb_parent;
}
static VALUE rb_font_set_out_color(VALUE self, VALUE out_color) {
  Font *ptr = convertFont(self);
  ptr->out_color->set(convertColor(out_color));
  return out_color;
}

static VALUE rb_font_default_name(VALUE) {
  return Font::default_name;
}
static VALUE rb_font_set_default_name(VALUE, VALUE default_name) {
  Font::default_name = default_name;
  return default_name;
}
static VALUE rb_font_default_size(VALUE) {
  return INT2NUM(Font::default_size);
}
static VALUE rb_font_set_default_size(VALUE, VALUE default_size) {
  Font::default_size = NUM2INT(default_size);
  return default_size;
}
static VALUE rb_font_default_bold(VALUE) {
  return (Font::default_bold) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_default_bold(VALUE, VALUE default_bold) {
  Font::default_bold = RTEST(default_bold);
  return default_bold;
}
static VALUE rb_font_default_italic(VALUE) {
  return (Font::default_italic) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_default_italic(VALUE, VALUE default_italic) {
  Font::default_italic = RTEST(default_italic);
  return default_italic;
}
static VALUE rb_font_default_shadow(VALUE) {
  return (Font::default_shadow) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_default_shadow(VALUE, VALUE default_shadow) {
  Font::default_shadow = RTEST(default_shadow);
  return default_shadow;
}
static VALUE rb_font_default_outline(VALUE) {
  return (Font::default_outline) ? Qtrue : Qfalse;
}
static VALUE rb_font_set_default_outline(VALUE, VALUE default_outline) {
  Font::default_outline = RTEST(default_outline);
  return default_outline;
}
static VALUE rb_font_default_color(VALUE) {
  return (Font::default_color)->rb_parent;
}
static VALUE rb_font_set_default_color(VALUE, VALUE default_color) {
  Font::default_color->set(convertColor(default_color));
  return default_color;
}
static VALUE rb_font_default_out_color(VALUE) {
  return (Font::default_out_color)->rb_parent;
}
static VALUE rb_font_set_default_out_color(VALUE, VALUE default_out_color) {
  Font::default_out_color->set(convertColor(default_out_color));
  return default_out_color;
}
