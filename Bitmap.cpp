#include <string>

#include <SDL_image.h>

#include "Bitmap.h"
#include "misc.h"
#include "file_misc.h"

void Bitmap::initialize(const char *filename) {
  SDL_RWops *rwops;
  rwops = openPath(filename);
  if(!rwops) rwops = openPath(std::string(filename)+".png");
  if(!rwops) rwops = openPath(std::string(filename)+".jpg");
  if(!rwops) rwops = openPath(std::string(filename)+".bmp");
  if(rwops == nullptr) {
    fprintf(stderr, "image %s not found\n", filename);
    SDL_Quit();
    exit(1);
  }
  // fprintf(stderr, "loaded %s.png\n", filename);
  this->surface = IMG_Load_RW(rwops, true);
  if(!this->surface) {
    fprintf(stderr, "Couldn't load image: %s\n", SDL_GetError());
    exit(1);
  }
  this->font = Font::create();
  this->texture = nullptr;
}
void Bitmap::initialize(int width, int height) {
  Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
  this->surface = SDL_CreateRGBSurface(
      0, width, height, 32, rmask, gmask, bmask, amask);
  this->font = Font::create();
  this->texture = nullptr;
}
void Bitmap::dispose() {
  if(this->surface) {
    SDL_FreeSurface(this->surface);
    this->surface = nullptr;
    if(this->texture) {
      SDL_DestroyTexture(this->texture);
      this->texture = nullptr;
    }
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
  invalidateTexture();
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(
      renderer, color->red, color->green, color->blue, color->alpha);
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;
  SDL_RenderFillRect(renderer, &rect);
  SDL_DestroyRenderer(renderer);
}
void Bitmap::fill_rect(Rect *rect, Color *color) {
  fill_rect(rect->x, rect->y, rect->width, rect->height, color);
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
  invalidateTexture();
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(renderer, NULL);
  SDL_DestroyRenderer(renderer);
}
void Bitmap::clear_rect(int x, int y, int width, int height) {
  invalidateTexture();
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;
  SDL_RenderFillRect(renderer, &rect);
  SDL_DestroyRenderer(renderer);
}
void Bitmap::clear_rect(Rect *rect) {
  clear_rect(rect->x, rect->y, rect->width, rect->height);
}
Color *Bitmap::get_pixel(int x, int y) {
  SDL_LockSurface(surface);
  SDL_PixelFormat *fmt = surface->format;
  Uint8 *ptr =
    (Uint8*)surface->pixels + y * surface->pitch + x * fmt->BytesPerPixel;
  Color *ret = nullptr;
  if(fmt->BytesPerPixel==1) {
    SDL_Color color = fmt->palette->colors[*ptr];
    ret = Color::create(color.r, color.g, color.b, color.a);
  } else if(fmt->BytesPerPixel==2) {
    // TODO
  } else if(fmt->BytesPerPixel==3) {
    // TODO
  } else if(fmt->BytesPerPixel==4) {
    // TODO
    Uint32 val = *(Uint32*)ptr;
    int red = ((val&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss;
    int green = ((val&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss;
    int blue = ((val&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss;
    int alpha = ((val&fmt->Amask)>>fmt->Ashift)<<fmt->Aloss;
    ret = Color::create(red, green, blue, alpha);
  }
  SDL_UnlockSurface(surface);
  return ret;
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
  if(!str[0]) return;
  TTF_Font *font = this->font->createTTFFont();
  if(!font) {
    fprintf(stderr, "Font Not Found\n");
    exit(1);
  }
  invalidateTexture();
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
  SDL_Color color;
  color.r = this->font->color->red;
  color.g = this->font->color->green;
  color.b = this->font->color->blue;
  color.a = this->font->color->alpha;
  SDL_Surface *text_surface = TTF_RenderUTF8_Blended(
      font, str, color);
  if(!text_surface) {
    fprintf(stderr, "Can't draw text: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    return;
  }
  SDL_Texture *text_texture = SDL_CreateTextureFromSurface(
      renderer, text_surface);
  if(!text_texture) {
    fprintf(stderr,
        "Can't get texture of rendered text: %s\n", SDL_GetError());
    SDL_FreeSurface(text_surface);
    SDL_DestroyRenderer(renderer);
    return;
  }
  SDL_Rect dst_rect;
  int w = text_surface->w;
  if(w >= width) {
    if(w * 0.6 >= width) {
      w = w * 0.6;
    } else {
      w = width;
    }
  }
  // TODO: shadow and outline
  dst_rect.x = x + (width-w)*align/2;
  dst_rect.y = y + (height-text_surface->h)/2;
  dst_rect.w = w;
  dst_rect.h = text_surface->h;
  SDL_SetTextureAlphaMod(text_texture, 255);
  // TODO: correct blend mode needed
  // SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_NONE);
  SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);
  SDL_DestroyTexture(text_texture);
  SDL_FreeSurface(text_surface);
  SDL_DestroyRenderer(renderer);
}
void Bitmap::draw_text(Rect *rect, const char *str,
    int align) {
  draw_text(rect->x, rect->y, rect->width, rect->height, str, align);
}
Rect *Bitmap::text_size(const char *str) {
  TTF_Font *font = this->font->createTTFFont();
  if(!font) {
    fprintf(stderr, "Font Not Found\n");
    exit(1);
  }
  int w, h;
  TTF_SizeUTF8(font, str, &w, &h);
  return Rect::create(0, 0, w, h);
}

SDL_Texture *Bitmap::createTexture(SDL_Renderer *renderer) {
  if(!this->texture) {
    this->texture = SDL_CreateTextureFromSurface(renderer, this->surface);
  }
  return this->texture;
}
void Bitmap::invalidateTexture() {
  if(this->texture) {
    SDL_DestroyTexture(this->texture);
    this->texture = nullptr;
  }
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
  return exportColor(ptr->get_pixel(NUM2INT(x), NUM2INT(y)));
}

static VALUE rb_bitmap_set_pixel(VALUE self, VALUE x, VALUE y, VALUE color) {
  Bitmap *ptr = convertBitmap(self);
  ptr->set_pixel(NUM2INT(x), NUM2INT(y), convertColor(color));
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
  VALUE str;
  switch(argc) {
    case 6:
      str = rb_check_convert_type(argv[4], T_STRING, "String", "to_s");
      ptr->draw_text(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), StringValueCStr(str), NUM2INT(argv[5]));
      break;
    case 5:
      str = rb_check_convert_type(argv[4], T_STRING, "String", "to_s");
      ptr->draw_text(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), StringValueCStr(str));
      break;
    case 3:
      str = rb_check_convert_type(argv[1], T_STRING, "String", "to_s");
      ptr->draw_text(
          convertRect(argv[0]), StringValueCStr(str), NUM2INT(argv[2]));
      break;
    case 2:
      str = rb_check_convert_type(argv[1], T_STRING, "String", "to_s");
      ptr->draw_text(
          convertRect(argv[0]), StringValueCStr(str));
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
  VALUE str2 = rb_check_convert_type(str, T_STRING, "String", "to_s");
  return exportRect(ptr->text_size(StringValueCStr(str2)));
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
