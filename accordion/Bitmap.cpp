#include <string>
#include <algorithm>

#include <SDL_image.h>

#include "Bitmap.h"
#include "misc.h"
#include "file_misc.h"
#include "sdl_misc.h"

void Bitmap::initialize(const char *filename) {
  SDL_RWops *rwops;
  rwops = openPath(filename);
  if(!rwops) rwops = openPath(std::string(filename)+".png");
  if(!rwops) rwops = openPath(std::string(filename)+".jpg");
  if(!rwops) rwops = openPath(std::string(filename)+".bmp");
  if(rwops == nullptr) {
    fprintf(stderr, "image %s not found\n", filename);
    quitSDL(1);
  }
  // fprintf(stderr, "loaded %s.png\n", filename);
  this->surface = IMG_Load_RW(rwops, true);
  if(!this->surface) {
    fprintf(stderr, "Couldn't load image: %s\n", SDL_GetError());
    quitSDL(1);
  }
  {
    SDL_Surface *old = this->surface;
    this->surface = SDL_ConvertSurfaceFormat(
        old, stdformat, 0);
    SDL_FreeSurface(old);
  }
  this->font = Font::create();
  this->texture = nullptr;
}
void Bitmap::initialize(int width, int height) {
  this->surface = SDL_CreateRGBSurface(
      0, width, height, 32, rmask, gmask, bmask, amask);
  {
    SDL_Surface *old = this->surface;
    this->surface = SDL_ConvertSurfaceFormat(
        old, stdformat, 0);
    SDL_FreeSurface(old);
  }
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
VALUE Bitmap::rect() {
  return rb_rect_new(0, 0, this->surface->w, this->surface->h);
}
static void blt(
    SDL_Surface *dst_surface, int dx, int dy,
    SDL_Surface *src_surface, int sx, int sy, int width, int height,
    int opacity) {
  int offx = sx-dx;
  int offy = sy-dy;
  int x1 = std::max<int>({dx, 0, -offx});
  int y1 = std::max<int>({dy, 0, -offy});
  int x2 = std::min<int>({dx+width, dst_surface->w, src_surface->w-offx});
  int y2 = std::min<int>({dy+height, dst_surface->h, src_surface->h-offy});
  SDL_LockSurface(dst_surface);
  SDL_LockSurface(src_surface);
  for(int xx = x1; xx < x2; ++xx) {
    for(int yy = y1; yy < y2; ++yy) {
      Uint32 *src_pixel =
        (Uint32*)((Uint8*)src_surface->pixels +
            (yy+offy) * src_surface->pitch + (xx+offx) * 4);
      Uint8 srcR8, srcG8, srcB8, srcA8;
      SDL_GetRGBA(
          *src_pixel, src_surface->format, &srcR8, &srcG8, &srcB8, &srcA8);
      double srcR = srcR8/255.0;
      double srcG = srcG8/255.0;
      double srcB = srcB8/255.0;
      double srcA = srcA8/255.0*(opacity/255.0);
      Uint32 *dst_pixel =
        (Uint32*)((Uint8*)dst_surface->pixels +
            yy * dst_surface->pitch + xx * 4);
      Uint8 dstR8, dstG8, dstB8, dstA8;
      SDL_GetRGBA(
          *dst_pixel, dst_surface->format, &dstR8, &dstG8, &dstB8, &dstA8);
      double dstR = dstR8/255.0;
      double dstG = dstG8/255.0;
      double dstB = dstB8/255.0;
      double dstA = dstA8/255.0;
      double newA = dstA*(1-srcA)+srcA;
      double newR = (dstR*(dstA*(1-srcA))+srcR*srcA)/newA;
      double newG = (dstG*(dstA*(1-srcA))+srcG*srcA)/newA;
      double newB = (dstB*(dstA*(1-srcA))+srcB*srcA)/newA;
      *dst_pixel =
        SDL_MapRGBA(
            dst_surface->format, newR*255, newG*255, newB*255, newA*255);
    }
  }
  SDL_UnlockSurface(src_surface);
  SDL_UnlockSurface(dst_surface);
}
static void stretch_blt(
    SDL_Surface *dst_surface, int dx, int dy, int dwidth, int dheight,
    SDL_Surface *src_surface, int sx, int sy, int swidth, int sheight,
    int opacity) {
  double scalex = (double)swidth/dwidth;
  double scaley = (double)sheight/dheight;
  int x1 = std::max<int>({dx, 0, dx-(int)(sx/scalex)});
  int y1 = std::max<int>({dy, 0, dy-(int)(sy/scaley)});
  int x2 = std::min<int>({
      dx+dwidth, dst_surface->w, dx+(int)((src_surface->w-sx)/scalex)});
  int y2 = std::min<int>({
      dy+dheight, dst_surface->h, dy+(int)((src_surface->h-sy)/scaley)});
  SDL_LockSurface(dst_surface);
  SDL_LockSurface(src_surface);
  for(int xx = x1; xx < x2; ++xx) {
    for(int yy = y1; yy < y2; ++yy) {
      Uint32 *src_pixel =
        (Uint32*)((Uint8*)src_surface->pixels +
            (int)((yy-dy)*scaley+sy) * src_surface->pitch +
            (int)((xx-dx)*scalex+sx) * 4);
      Uint8 srcR8, srcG8, srcB8, srcA8;
      SDL_GetRGBA(
          *src_pixel, src_surface->format, &srcR8, &srcG8, &srcB8, &srcA8);
      double srcR = srcR8/255.0;
      double srcG = srcG8/255.0;
      double srcB = srcB8/255.0;
      double srcA = srcA8/255.0*(opacity/255.0);
      Uint32 *dst_pixel =
        (Uint32*)((Uint8*)dst_surface->pixels +
            yy * dst_surface->pitch + xx * 4);
      Uint8 dstR8, dstG8, dstB8, dstA8;
      SDL_GetRGBA(
          *dst_pixel, dst_surface->format, &dstR8, &dstG8, &dstB8, &dstA8);
      double dstR = dstR8/255.0;
      double dstG = dstG8/255.0;
      double dstB = dstB8/255.0;
      double dstA = dstA8/255.0;
      double newA = dstA*(1-srcA)+srcA;
      double newR = (dstR*(dstA*(1-srcA))+srcR*srcA)/newA;
      double newG = (dstG*(dstA*(1-srcA))+srcG*srcA)/newA;
      double newB = (dstB*(dstA*(1-srcA))+srcB*srcA)/newA;
      *dst_pixel =
        SDL_MapRGBA(
            dst_surface->format, newR*255, newG*255, newB*255, newA*255);
    }
  }
  SDL_UnlockSurface(src_surface);
  SDL_UnlockSurface(dst_surface);
}
void Bitmap::blt(int x, int y, Bitmap *src_bitmap, VALUE src_rect,
    int opacity) {
  ::blt(surface, x, y, src_bitmap->surface,
      rb_rect_x(src_rect), rb_rect_y(src_rect),
      rb_rect_width(src_rect), rb_rect_height(src_rect),
      opacity);
  if(this->texture) {
    SDL_UpdateTexture(this->texture, NULL, surface->pixels, surface->pitch);
  }
}
void Bitmap::stretch_blt(VALUE dest_rect, Bitmap *src_bitmap, VALUE src_rect,
    int opacity) {
  ::stretch_blt(
      surface,
      rb_rect_x(dest_rect), rb_rect_y(dest_rect),
      rb_rect_width(dest_rect), rb_rect_height(dest_rect),
      src_bitmap->surface,
      rb_rect_x(src_rect), rb_rect_y(src_rect),
      rb_rect_width(src_rect), rb_rect_height(src_rect),
      opacity);
  if(this->texture) {
    SDL_UpdateTexture(this->texture, NULL, surface->pixels, surface->pitch);
  }
}
void Bitmap::fill_rect(int x, int y, int width, int height, VALUE color) {
  int x1 = std::max(x, 0);
  int y1 = std::max(y, 0);
  int x2 = std::min(x+width, surface->w);
  int y2 = std::min(y+height, surface->h);
  double srcR = rb_color_red(color) / 255.0;
  double srcG = rb_color_green(color) / 255.0;
  double srcB = rb_color_blue(color) / 255.0;
  double srcA = rb_color_alpha(color) / 255.0;
  SDL_LockSurface(surface);
  for(int xx = x1; xx < x2; ++xx) {
    for(int yy = y1; yy < y2; ++yy) {
      Uint32 *pixel =
        (Uint32*)((Uint8*)surface->pixels +
            yy * surface->pitch + xx * 4);
      Uint8 dstR, dstG, dstB, dstA;
      SDL_GetRGBA(*pixel, surface->format, &dstR, &dstG, &dstB, &dstA);
      double newA = dstA/255.0*(1-srcA)+srcA;
      double newR = (dstR/255.0*(dstA/255.0*(1-srcA))+srcR*srcA)/newA;
      double newG = (dstG/255.0*(dstA/255.0*(1-srcA))+srcG*srcA)/newA;
      double newB = (dstB/255.0*(dstA/255.0*(1-srcA))+srcB*srcA)/newA;
      *pixel =
        SDL_MapRGBA(surface->format, newR*255, newG*255, newB*255, newA*255);
    }
  }
  SDL_UnlockSurface(surface);
  if(this->texture) {
    SDL_Rect rect = {x1, y1, x2-x2, y2-y1};
    SDL_UpdateTexture(this->texture, &rect, surface->pixels, surface->pitch);
  }
}
void Bitmap::fill_rect(VALUE rect, VALUE color) {
  fill_rect(
      rb_rect_x(rect), rb_rect_y(rect),
      rb_rect_width(rect), rb_rect_height(rect), color);
}
void Bitmap::gradient_fill_rect(
    int x, int y, int width, int height, VALUE color1, VALUE color2,
    bool vertical) {
  int x1 = std::max(x, 0);
  int y1 = std::max(y, 0);
  int x2 = std::min(x+width, surface->w);
  int y2 = std::min(y+height, surface->h);
  double src1R = rb_color_red(color1) / 255.0;
  double src1G = rb_color_green(color1) / 255.0;
  double src1B = rb_color_blue(color1) / 255.0;
  double src1A = rb_color_alpha(color1) / 255.0;
  double src2R = rb_color_red(color2) / 255.0;
  double src2G = rb_color_green(color2) / 255.0;
  double src2B = rb_color_blue(color2) / 255.0;
  double src2A = rb_color_alpha(color2) / 255.0;
  SDL_LockSurface(surface);
  for(int xx = x1; xx < x2; ++xx) {
    for(int yy = y1; yy < y2; ++yy) {
      double gratio = vertical ? (double)(yy-y1)/(y2-y1) :
        (double)(xx-x1)/(x2-x1);
      double srcR = src1R*(1-gratio)+src2R*gratio;
      double srcG = src1G*(1-gratio)+src2G*gratio;
      double srcB = src1B*(1-gratio)+src2B*gratio;
      double srcA = src1A*(1-gratio)+src2A*gratio;
      Uint32 *pixel =
        (Uint32*)((Uint8*)surface->pixels +
            yy * surface->pitch + xx * 4);
      Uint8 dstR, dstG, dstB, dstA;
      SDL_GetRGBA(*pixel, surface->format, &dstR, &dstG, &dstB, &dstA);
      double newA = dstA/255.0*(1-srcA)+srcA;
      double newR = (dstR/255.0*(dstA/255.0*(1-srcA))+srcR*srcA)/newA;
      double newG = (dstG/255.0*(dstA/255.0*(1-srcA))+srcG*srcA)/newA;
      double newB = (dstB/255.0*(dstA/255.0*(1-srcA))+srcB*srcA)/newA;
      *pixel =
        SDL_MapRGBA(surface->format, newR*255, newG*255, newB*255, newA*255);
    }
  }
  SDL_UnlockSurface(surface);
  if(this->texture) {
    SDL_Rect rect = {x1, y1, x2-x2, y2-y1};
    SDL_UpdateTexture(this->texture, &rect, surface->pixels, surface->pitch);
  }
}
void Bitmap::gradient_fill_rect(
    VALUE rect, VALUE color1, VALUE color2,
    bool vertical) {
  gradient_fill_rect(
      rb_rect_x(rect), rb_rect_y(rect),
      rb_rect_width(rect), rb_rect_height(rect),
      color1, color2, vertical);
}
void Bitmap::clear() {
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(renderer, NULL);
  SDL_DestroyRenderer(renderer);
  if(this->texture) {
    SDL_Rect rect = {0, 0, surface->w, surface->h};
    SDL_UpdateTexture(this->texture, &rect, surface->pixels, surface->pitch);
  }
}
void Bitmap::clear_rect(int x, int y, int width, int height) {
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
  if(this->texture) {
    SDL_UpdateTexture(this->texture, &rect, surface->pixels, surface->pitch);
  }
}
void Bitmap::clear_rect(VALUE rect) {
  clear_rect(
      rb_rect_x(rect), rb_rect_y(rect),
      rb_rect_width(rect), rb_rect_height(rect));
}
VALUE Bitmap::get_pixel(int x, int y) {
  SDL_LockSurface(surface);
  Uint32 *pixel =
    (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * 4);
  Uint8 red, green, blue, alpha;
  SDL_GetRGBA(*pixel, surface->format, &red, &green, &blue, &alpha);
  VALUE ret = rb_color_new(red, green, blue, alpha);
  SDL_UnlockSurface(surface);
  return ret;
}
void Bitmap::set_pixel(int x, int y, VALUE color) {
  SDL_LockSurface(surface);
  Uint32 *pixel =
    (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * 4);
  *pixel = SDL_MapRGBA(
      surface->format,
      rb_color_red(color),
      rb_color_green(color),
      rb_color_blue(color),
      rb_color_alpha(color));
  SDL_UnlockSurface(surface);
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
    quitSDL(1);
  }
  SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
  SDL_Color color;
  color.r = rb_color_red(this->font->color);
  color.g = rb_color_green(this->font->color);
  color.b = rb_color_blue(this->font->color);
  color.a = rb_color_alpha(this->font->color);
  SDL_Surface *text_surface = TTF_RenderUTF8_Blended(
      font, str, color);
  if(!text_surface) {
    fprintf(stderr, "Can't draw text: %s\n", SDL_GetError());
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
  ::stretch_blt(
      surface,
      dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h,
      text_surface,
      0, 0, text_surface->w, text_surface->h, 255);
  SDL_FreeSurface(text_surface);
  SDL_DestroyRenderer(renderer);
  if(this->texture) {
    SDL_UpdateTexture(
        this->texture, NULL, this->surface->pixels, this->surface->pitch);
  }
}
void Bitmap::draw_text(VALUE rect, const char *str,
    int align) {
  draw_text(
      rb_rect_x(rect), rb_rect_y(rect),
      rb_rect_width(rect), rb_rect_height(rect), str, align);
}
VALUE Bitmap::text_size(const char *str) {
  TTF_Font *font = this->font->createTTFFont();
  if(!font) {
    fprintf(stderr, "Font Not Found\n");
    quitSDL(1);
  }
  int w, h;
  TTF_SizeUTF8(font, str, &w, &h);
  return rb_rect_new(0, 0, w, h);
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
  ptr->surface = nullptr;
  ptr->texture = nullptr;
  ptr->font = nullptr;
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
  return ptr->rect();
}
static VALUE rb_bitmap_blt(int argc, VALUE *argv, VALUE self) {
  Bitmap *ptr = convertBitmap(self);
  switch(argc) {
    case 5:
      ptr->blt(
          NUM2INT(argv[0]), NUM2INT(argv[1]), convertBitmap(argv[2]),
          argv[3], NUM2INT(argv[4]));
      break;
    case 4:
      ptr->blt(
          NUM2INT(argv[0]), NUM2INT(argv[1]), convertBitmap(argv[2]),
          argv[3]);
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
          argv[0], convertBitmap(argv[1]),
          argv[2], NUM2INT(argv[3]));
      break;
    case 3:
      ptr->stretch_blt(argv[0], convertBitmap(argv[1]), argv[2]);
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
          NUM2INT(argv[3]), argv[4]);
      break;
    case 2:
      ptr->fill_rect(argv[0], argv[1]);
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
          NUM2INT(argv[3]), argv[4], argv[5],
          RTEST(argv[6]));
      break;
    case 6:
      ptr->gradient_fill_rect(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]), argv[4], argv[5]);
      break;
    case 4:
      ptr->gradient_fill_rect(argv[0], argv[1], argv[2], RTEST(argv[3]));
      break;
    case 3:
      ptr->gradient_fill_rect(argv[0], argv[1], argv[2]);
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
      ptr->clear_rect(argv[0]);
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
  return ptr->get_pixel(NUM2INT(x), NUM2INT(y));
}

static VALUE rb_bitmap_set_pixel(VALUE self, VALUE x, VALUE y, VALUE color) {
  Bitmap *ptr = convertBitmap(self);
  ptr->set_pixel(NUM2INT(x), NUM2INT(y), color);
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
      ptr->draw_text(argv[0], StringValueCStr(str), NUM2INT(argv[2]));
      break;
    case 2:
      str = rb_check_convert_type(argv[1], T_STRING, "String", "to_s");
      ptr->draw_text(argv[0], StringValueCStr(str));
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
  return ptr->text_size(StringValueCStr(str2));
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
