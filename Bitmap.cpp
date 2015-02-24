#include <string>

#include <SDL_image.h>

#include "Bitmap.h"
#include "misc.h"

void Bitmap::initialize(const char *filename) {
  SDL_RWops *rwops;
  rwops = SDL_RWFromFile(
      (std::string(filename)+".png").c_str(), "rb");
  if(rwops == nullptr)
  rwops = SDL_RWFromFile(
      ("/home/qnighy/workdir/vxace/RTP/RPGVXAce/"+std::string(filename)+".png").c_str(), "rb");
  if(rwops == nullptr) {
    fprintf(stderr, "%s.png not found\n", filename);
    SDL_Quit();
    exit(1);
  }
  this->surface = IMG_Load_RW(rwops, true);
  this->font = Font::create();
}
void Bitmap::initialize(int width, int height) {
  this->surface = SDL_CreateRGBSurface(0,width,height,32,0,0,0,0);
  this->font = Font::create();
}
void Bitmap::dispose() {
  if(this->surface) {
    SDL_FreeSurface(this->surface);
    this->surface = nullptr;
  }
}
bool Bitmap::disposed() {
  return this->surface == nullptr;
}
int Bitmap::width() {
  return this->surface->w;
}
int Bitmap::height() {
  return this->surface->h;
}
Rect *Bitmap::rect() {
  return Rect::create(0, 0, this->surface->w, this->surface->h);
}
void Bitmap::blt(int x, int y, Bitmap *src_bitmap, Rect *src_rect,
    int opacity) {
  fprintf(stderr, "TODO: Bitmap::blt\n");
}
void Bitmap::stretch_blt(Rect *dest_rect, Bitmap *src_bitmap, Rect *src_rect,
    int opacity) {
  fprintf(stderr, "TODO: Bitmap::stretch_blt\n");
}
void Bitmap::fill_rect(int x, int y, int width, int height, Color *color) {
  fprintf(stderr, "TODO: Bitmap::fill_rect\n");
}
void Bitmap::fill_rect(Rect *rect, Color *color) {
  fprintf(stderr, "TODO: Bitmap::fill_rect\n");
}
void Bitmap::gradient_fill_rect(
    int x, int y, int width, int height, Color *color1, Color *color2,
    bool vertical) {
  fprintf(stderr, "TODO: Bitmap::gradient_fill_rect\n");
}
void Bitmap::gradient_fill_rect(
    Rect *rect, Color *color1, Color *color2,
    bool vertical) {
  fprintf(stderr, "TODO: Bitmap::gradient_fill_rect\n");
}
void Bitmap::clear() {
  fprintf(stderr, "TODO: Bitmap::clear\n");
}
void Bitmap::clear_rect(int x, int y, int width, int height) {
  fprintf(stderr, "TODO: Bitmap::clear_rect\n");
}
void Bitmap::clear_rect(Rect *rect) {
  fprintf(stderr, "TODO: Bitmap::clear_rect\n");
}
Color *Bitmap::get_pixel(int x, int y) {
  fprintf(stderr, "TODO: Bitmap::get_pixel\n");
  return Color::create();
}
void Bitmap::set_pixel(int x, int y, Color *color) {
  fprintf(stderr, "TODO: Bitmap::set_pixel\n");
}
void Bitmap::hue_change(int hue) {
  fprintf(stderr, "TODO: Bitmap::hue_change\n");
}
void Bitmap::blur() {
  fprintf(stderr, "TODO: Bitmap::blur\n");
}
void Bitmap::radial_blur(int angle, int division) {
  fprintf(stderr, "TODO: Bitmap::radial_blur\n");
}
void Bitmap::draw_text(int x, int y, int width, int height, const char *str,
    int align) {
  fprintf(stderr, "TODO: Bitmap::draw_text\n");
}
void Bitmap::draw_text(Rect *rect, const char *str,
    int align) {
  fprintf(stderr, "TODO: Bitmap::draw_text\n");
}
Rect *Bitmap::text_size(const char *str) {
  fprintf(stderr, "TODO: Bitmap::text_size\n");
  return Rect::create();
}

static void bitmap_mark(Bitmap *);
static VALUE bitmap_alloc(VALUE klass);

static VALUE rb_bitmap_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_bitmap_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_bitmap_dispose(VALUE self);
static VALUE rb_bitmap_disposed(VALUE self);
static VALUE rb_bitmap_width(VALUE self);
static VALUE rb_bitmap_height(VALUE self);
static VALUE rb_bitmap_rect(VALUE self);
static VALUE rb_bitmap_blt(int argc, VALUE *argv, VALUE self);
static VALUE rb_bitmap_stretch_blt(int argc, VALUE *argv, VALUE self);
static VALUE rb_bitmap_fill_rect(int argc, VALUE *argv, VALUE self);
static VALUE rb_bitmap_gradient_fill_rect(int argc, VALUE *argv, VALUE self);
static VALUE rb_bitmap_clear(VALUE self);
static VALUE rb_bitmap_clear_rect(int argc, VALUE *argv, VALUE self);
static VALUE rb_bitmap_get_pixel(VALUE self, VALUE x, VALUE y);
static VALUE rb_bitmap_set_pixel(VALUE self, VALUE x, VALUE y, VALUE color);
static VALUE rb_bitmap_hue_change(VALUE self, VALUE hue);
static VALUE rb_bitmap_blur(VALUE self);
static VALUE rb_bitmap_radial_blur(VALUE self, VALUE angle, VALUE division);
static VALUE rb_bitmap_draw_text(int argc, VALUE *argv, VALUE self);
static VALUE rb_bitmap_text_size(VALUE self, VALUE str);
static VALUE rb_bitmap_font(VALUE self);
static VALUE rb_bitmap_set_font(VALUE self, VALUE font);

VALUE rb_cBitmap;
void InitBitmap() {
  rb_cBitmap = rb_define_class("Bitmap", rb_cObject);
  rb_define_alloc_func(rb_cBitmap, bitmap_alloc);
  rb_define_method(rb_cBitmap, "initialize",
      (VALUE(*)(...))rb_bitmap_initialize, -1);
  rb_define_method(rb_cBitmap, "initialize_copy",
      (VALUE(*)(...))rb_bitmap_initialize_copy, 1);

  rb_define_method(rb_cBitmap, "dispose",
      (VALUE(*)(...))rb_bitmap_dispose, 0);
  rb_define_method(rb_cBitmap, "disposed?",
      (VALUE(*)(...))rb_bitmap_disposed, 0);
  rb_define_method(rb_cBitmap, "width",
      (VALUE(*)(...))rb_bitmap_width, 0);
  rb_define_method(rb_cBitmap, "height",
      (VALUE(*)(...))rb_bitmap_height, 0);
  rb_define_method(rb_cBitmap, "rect",
      (VALUE(*)(...))rb_bitmap_rect, 0);
  rb_define_method(rb_cBitmap, "blt",
      (VALUE(*)(...))rb_bitmap_blt, -1);
  rb_define_method(rb_cBitmap, "stretch_blt",
      (VALUE(*)(...))rb_bitmap_stretch_blt, -1);
  rb_define_method(rb_cBitmap, "fill_rect",
      (VALUE(*)(...))rb_bitmap_fill_rect, -1);
  rb_define_method(rb_cBitmap, "gradient_fill_rect",
      (VALUE(*)(...))rb_bitmap_gradient_fill_rect, -1);
  rb_define_method(rb_cBitmap, "clear",
      (VALUE(*)(...))rb_bitmap_clear, 0);
  rb_define_method(rb_cBitmap, "clear_rect",
      (VALUE(*)(...))rb_bitmap_clear_rect, -1);
  rb_define_method(rb_cBitmap, "get_pixel",
      (VALUE(*)(...))rb_bitmap_get_pixel, 2);
  rb_define_method(rb_cBitmap, "set_pixel",
      (VALUE(*)(...))rb_bitmap_set_pixel, 3);
  rb_define_method(rb_cBitmap, "hue_change",
      (VALUE(*)(...))rb_bitmap_hue_change, 1);
  rb_define_method(rb_cBitmap, "blur",
      (VALUE(*)(...))rb_bitmap_blur, 0);
  rb_define_method(rb_cBitmap, "radial_blur",
      (VALUE(*)(...))rb_bitmap_radial_blur, 2);
  rb_define_method(rb_cBitmap, "draw_text",
      (VALUE(*)(...))rb_bitmap_draw_text, -1);
  rb_define_method(rb_cBitmap, "text_size",
      (VALUE(*)(...))rb_bitmap_text_size, 1);

  rb_define_method(rb_cBitmap, "font",
      (VALUE(*)(...))rb_bitmap_font, 0);
  rb_define_method(rb_cBitmap, "font=",
      (VALUE(*)(...))rb_bitmap_set_font, 1);
}

Bitmap *convertBitmap(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))bitmap_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Bitmap",
        rb_class2name(rb_obj_class(obj)));
  }
  Bitmap *ret;
  Data_Get_Struct(obj, Bitmap, ret);
  return ret;
}
Bitmap *convertBitmapOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertBitmap(obj);
}
VALUE exportBitmap(Bitmap *ptr) {
  return ptr ? ptr->rb_parent : Qnil;
}

Bitmap *Bitmap::create(const char *filename) {
  VALUE ret = bitmap_alloc(rb_cBitmap);
  Bitmap *ptr = convertBitmap(ret);
  ptr->initialize(filename);
  return ptr;
}
Bitmap *Bitmap::create(int width, int height) {
  VALUE ret = bitmap_alloc(rb_cBitmap);
  Bitmap *ptr = convertBitmap(ret);
  ptr->initialize(width, height);
  return ptr;
}

static void bitmap_mark(Bitmap *ptr) {
  rb_gc_mark(ptr->font->rb_parent);
}

static void bitmap_free(Bitmap *ptr) {
  ptr->dispose();
  xfree(ptr);
}

static VALUE bitmap_alloc(VALUE klass) {
  Bitmap *ptr = ALLOC(Bitmap);
  VALUE ret = Data_Wrap_Struct(klass, bitmap_mark, bitmap_free, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_bitmap_initialize(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 2:
      ptr->initialize(NUM2INT(argv[0]), NUM2INT(argv[1]));
      break;
    case 1:
      ptr->initialize(StringValueCStr(argv[0]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1 or 2)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_bitmap_initialize_copy(VALUE self, VALUE orig) {
  Bitmap *ptr = convertBitmap(self);
  Bitmap *orig_ptr = convertBitmap(orig);
  ptr->surface = SDL_CreateRGBSurface(
      0, orig_ptr->width(), orig_ptr->height(), 32, 0, 0, 0, 0);
  SDL_BlitSurface(orig_ptr->surface, NULL, ptr->surface, NULL);
  ptr->font = Font::create();
  ptr->font->set(orig_ptr->font);
  return Qnil;
}

static VALUE rb_bitmap_dispose(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  ptr->dispose();
  return Qnil;
}
static VALUE rb_bitmap_disposed(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  return (ptr->disposed()) ? Qtrue : Qfalse;
}
static VALUE rb_bitmap_width(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  return INT2NUM(ptr->width());
}
static VALUE rb_bitmap_height(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  return INT2NUM(ptr->height());
}
static VALUE rb_bitmap_rect(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  return exportRect(ptr->rect());
}
static VALUE rb_bitmap_blt(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 5:
      ptr->blt(
          NUM2INT(argv[0]), NUM2INT(argv[1]), convertBitmap(argv[2]),
          convertRect(argv[3]), NUM2INT(argv[4]));
      break;
    case 4:
      ptr->blt(
          NUM2INT(argv[0]), NUM2INT(argv[1]), convertBitmap(argv[2]),
          convertRect(argv[3]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 4..5)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_bitmap_stretch_blt(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 4:
      ptr->stretch_blt(
          convertRect(argv[0]), convertBitmap(argv[1]),
          convertRect(argv[2]), NUM2INT(argv[3]));
      break;
    case 3:
      ptr->stretch_blt(
          convertRect(argv[0]), convertBitmap(argv[1]),
          convertRect(argv[2]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 3..4)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_bitmap_fill_rect(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 5:
      ptr->fill_rect(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), convertColor(argv[4]));
      break;
    case 2:
      ptr->fill_rect(
          convertRect(argv[0]), convertColor(argv[1]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 2 or 5)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_bitmap_gradient_fill_rect(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 7:
      ptr->gradient_fill_rect(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), convertColor(argv[4]), convertColor(argv[5]),
          RTEST(argv[6]));
      break;
    case 6:
      ptr->gradient_fill_rect(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), convertColor(argv[4]), convertColor(argv[5]));
      break;
    case 4:
      ptr->gradient_fill_rect(
          convertRect(argv[0]), convertColor(argv[1]),
          convertColor(argv[2]), RTEST(argv[3]));
      break;
    case 3:
      ptr->gradient_fill_rect(
          convertRect(argv[0]), convertColor(argv[1]),
          convertColor(argv[2]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 3..4 or 6..7)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_bitmap_clear(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  ptr->clear();
  return Qnil;
}
static VALUE rb_bitmap_clear_rect(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 4:
      ptr->clear_rect(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]));
      break;
    case 1:
      ptr->clear_rect(convertRect(argv[0]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 1 or 4)", argc);
      break;
  }
  return Qnil;
}

static VALUE rb_bitmap_get_pixel(VALUE self, VALUE x, VALUE y) {
  Bitmap *ptr = convertBitmap(self);
  return exportColor(ptr->get_pixel(x, y));
}

static VALUE rb_bitmap_set_pixel(VALUE self, VALUE x, VALUE y, VALUE color) {
  Bitmap *ptr = convertBitmap(self);
  ptr->set_pixel(x, y, convertColor(color));
  return color;
}
static VALUE rb_bitmap_hue_change(VALUE self, VALUE hue) {
  Bitmap *ptr = convertBitmap(self);
  ptr->hue_change(NUM2INT(hue));
  return Qnil;
}
static VALUE rb_bitmap_blur(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  ptr->blur();
  return Qnil;
}
static VALUE rb_bitmap_radial_blur(VALUE self, VALUE angle, VALUE division) {
  Bitmap *ptr = convertBitmap(self);
  ptr->radial_blur(NUM2INT(angle), NUM2INT(division));
  return Qnil;
}
static VALUE rb_bitmap_draw_text(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 6:
      ptr->draw_text(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), StringValueCStr(argv[4]), NUM2INT(argv[5]));
      break;
    case 5:
      ptr->draw_text(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), StringValueCStr(argv[4]));
      break;
    case 3:
      ptr->draw_text(
          convertRect(argv[0]), StringValueCStr(argv[1]), NUM2INT(argv[2]));
      break;
    case 2:
      ptr->draw_text(
          convertRect(argv[0]), StringValueCStr(argv[1]));
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 2..3 or 5..6)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_bitmap_text_size(VALUE self, VALUE str) {
  Bitmap *ptr = convertBitmap(self);
  return exportRect(ptr->text_size(StringValueCStr(str)));
}

static VALUE rb_bitmap_font(VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  return exportFont(ptr->font);
}
static VALUE rb_bitmap_set_font(VALUE self, VALUE font) {
  Bitmap *ptr = convertBitmap(self);
  ptr->font->set(convertFont(font));
  return font;
}
