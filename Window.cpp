#include <string>

#include <SDL_image.h>

#include "Window.h"
#include "Graphics.h"
#include "misc.h"

void Window::initialize(int x, int y, int width, int height) {
  this->windowskin = nullptr;
  this->contents = Bitmap::create(1,1);
  this->cursor_rect = Rect::create();
  this->viewport = nullptr;
  this->active = true;
  this->visible = true;
  this->arrows_visible = true;
  this->pause = false;
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
  this->z = 100;
  this->ox = 0;
  this->oy = 0;
  this->padding = 12;
  this->padding_bottom = 12;
  this->opacity = 255;
  this->back_opacity = 192;
  this->contents_opacity = 255;
  this->openness = 255;
  this->tone = Tone::create();

  this->is_disposed = false;
  this->renderable_entry.type = RenderableType::WINDOW;
  this->renderable_entry.y = 0;
  this->renderable_entry.z = this->z;
  this->renderable_entry.renderable_id = current_renderable_id++;
  Graphics::register_renderable((Renderable*)this, this->viewport);
}
void Window::initialize() {
  initialize(0, 0, 0, 0);
}
void Window::dispose() {
  if(!this->is_disposed) {
    Graphics::unregister_renderable((Renderable*)this, this->viewport);
    this->is_disposed = true;
  }
}
bool Window::disposed() {
  return this->is_disposed;
}
void Window::update() {
  // TODO: Window::update
}
void Window::move(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}
bool Window::open_p() {
  return this->openness == 255;
}
bool Window::close_p() {
  return this->openness == 0;
}

void Window::render(
    SDL_Renderer *renderer,
    int rox, int roy, int rwidth, int rheight) {
  if(!this->visible) return;
  // TODO: Window::render
  // fprintf(stderr, "render!\n");
  if(this->windowskin && this->windowskin->surface) {
    int x = this->x;
    int y = this->y + this->height*(255-this->openness)/510;
    int width = this->width;
    int height = this->height*this->openness/255;
    if(width <= 4 || height <= 4) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, this->back_opacity);
    SDL_Rect rect;
    rect.x = x-rox+2;
    rect.y = y-roy+2;
    rect.w = width-4;
    rect.h = height-4;
    SDL_RenderFillRect(renderer, &rect);
    if(tone->red != 0.0 || tone->green != 0.0 ||
        tone->blue != 0.0 || tone->gray != 0.0) {
      // fprintf(stderr, "TODO: Window::render: tone\n");
    }
  }

  if(openness != 255) return;

  {
    SDL_Rect cursor_rect;
    cursor_rect.x = x-rox+padding-ox+this->cursor_rect->x;
    cursor_rect.y = y-roy+padding-oy+this->cursor_rect->y;
    cursor_rect.w = this->cursor_rect->width;
    cursor_rect.h = this->cursor_rect->height;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, active ? 100 : 50);
    SDL_RenderFillRect(renderer, &cursor_rect);
  }

  SDL_Rect src_rect_orig;
  src_rect_orig.x = ox;
  src_rect_orig.y = oy;
  src_rect_orig.w = width-padding-padding;
  src_rect_orig.h = height-padding-padding_bottom;
  SDL_Rect img_rect;
  img_rect.x = 0;
  img_rect.y = 0;
  img_rect.w = this->contents->surface->w;
  img_rect.h = this->contents->surface->h;
  SDL_Rect src_rect;
  SDL_IntersectRect(&src_rect_orig, &img_rect, &src_rect);
  SDL_Rect dst_rect;
  dst_rect.x = x-rox+padding-ox+src_rect.x;
  dst_rect.y = y-roy+padding-oy+src_rect.y;
  dst_rect.w = src_rect.w;
  dst_rect.h = src_rect.h;
  SDL_Texture *texture = this->contents->createTexture(renderer);
  SDL_SetTextureAlphaMod(texture, contents_opacity);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  // SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
  SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);
  // SDL_RenderCopy(renderer, texture, NULL, NULL);
}

static void window_mark(Window *);
static VALUE window_alloc(VALUE klass);

static VALUE rb_window_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_window_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_window_dup(VALUE self);
static VALUE rb_window_clone(VALUE self);

static VALUE rb_window_dispose(VALUE self);
static VALUE rb_window_disposed(VALUE self);
static VALUE rb_window_update(VALUE self);
static VALUE rb_window_move(
    VALUE self, VALUE x, VALUE y, VALUE width, VALUE height);
static VALUE rb_window_open_p(VALUE self);
static VALUE rb_window_close_p(VALUE self);
static VALUE rb_window_windowskin(VALUE self);
static VALUE rb_window_set_windowskin(VALUE self, VALUE windowskin);
static VALUE rb_window_contents(VALUE self);
static VALUE rb_window_set_contents(VALUE self, VALUE contents);
static VALUE rb_window_cursor_rect(VALUE self);
static VALUE rb_window_set_cursor_rect(VALUE self, VALUE cursor_rect);
static VALUE rb_window_viewport(VALUE self);
static VALUE rb_window_set_viewport(VALUE self, VALUE viewport);
static VALUE rb_window_active(VALUE self);
static VALUE rb_window_set_active(VALUE self, VALUE active);
static VALUE rb_window_visible(VALUE self);
static VALUE rb_window_set_visible(VALUE self, VALUE visible);
static VALUE rb_window_arrows_visible(VALUE self);
static VALUE rb_window_set_arrows_visible(VALUE self, VALUE arrows_visible);
static VALUE rb_window_pause(VALUE self);
static VALUE rb_window_set_pause(VALUE self, VALUE pause);
static VALUE rb_window_x(VALUE self);
static VALUE rb_window_set_x(VALUE self, VALUE x);
static VALUE rb_window_y(VALUE self);
static VALUE rb_window_set_y(VALUE self, VALUE y);
static VALUE rb_window_width(VALUE self);
static VALUE rb_window_set_width(VALUE self, VALUE width);
static VALUE rb_window_height(VALUE self);
static VALUE rb_window_set_height(VALUE self, VALUE height);
static VALUE rb_window_z(VALUE self);
static VALUE rb_window_set_z(VALUE self, VALUE z);
static VALUE rb_window_ox(VALUE self);
static VALUE rb_window_set_ox(VALUE self, VALUE ox);
static VALUE rb_window_oy(VALUE self);
static VALUE rb_window_set_oy(VALUE self, VALUE oy);
static VALUE rb_window_padding(VALUE self);
static VALUE rb_window_set_padding(VALUE self, VALUE padding);
static VALUE rb_window_padding_bottom(VALUE self);
static VALUE rb_window_set_padding_bottom(VALUE self, VALUE padding_bottom);
static VALUE rb_window_opacity(VALUE self);
static VALUE rb_window_set_opacity(VALUE self, VALUE opacity);
static VALUE rb_window_back_opacity(VALUE self);
static VALUE rb_window_set_back_opacity(VALUE self, VALUE back_opacity);
static VALUE rb_window_contents_opacity(VALUE self);
static VALUE rb_window_set_contents_opacity(VALUE self, VALUE contents_opacity);
static VALUE rb_window_openness(VALUE self);
static VALUE rb_window_set_openness(VALUE self, VALUE openness);
static VALUE rb_window_tone(VALUE self);
static VALUE rb_window_set_tone(VALUE self, VALUE tone);

VALUE rb_cWindow;
void InitWindow() {
  rb_cWindow = rb_define_class("Window", rb_cObject);
  rb_define_alloc_func(rb_cWindow, window_alloc);
  rb_define_method(rb_cWindow, "initialize",
      (VALUE(*)(...))rb_window_initialize, -1);
  rb_define_method(rb_cWindow, "initialize_copy",
      (VALUE(*)(...))rb_window_initialize_copy, 1);

  rb_define_method(rb_cWindow, "dup",
      (VALUE(*)(...))rb_window_dup, 1);
  rb_define_method(rb_cWindow, "clone",
      (VALUE(*)(...))rb_window_clone, 1);

  rb_define_method(rb_cWindow, "dispose",
      (VALUE(*)(...))rb_window_dispose, 0);
  rb_define_method(rb_cWindow, "disposed?",
      (VALUE(*)(...))rb_window_disposed, 0);
  rb_define_method(rb_cWindow, "update",
      (VALUE(*)(...))rb_window_update, 0);
  rb_define_method(rb_cWindow, "move",
      (VALUE(*)(...))rb_window_move, 4);
  rb_define_method(rb_cWindow, "open?",
      (VALUE(*)(...))rb_window_open_p, 0);
  rb_define_method(rb_cWindow, "close?",
      (VALUE(*)(...))rb_window_close_p, 0);

  rb_define_method(rb_cWindow, "windowskin",
      (VALUE(*)(...))rb_window_windowskin, 0);
  rb_define_method(rb_cWindow, "windowskin=",
      (VALUE(*)(...))rb_window_set_windowskin, 1);
  rb_define_method(rb_cWindow, "contents",
      (VALUE(*)(...))rb_window_contents, 0);
  rb_define_method(rb_cWindow, "contents=",
      (VALUE(*)(...))rb_window_set_contents, 1);
  rb_define_method(rb_cWindow, "cursor_rect",
      (VALUE(*)(...))rb_window_cursor_rect, 0);
  rb_define_method(rb_cWindow, "cursor_rect=",
      (VALUE(*)(...))rb_window_set_cursor_rect, 1);
  rb_define_method(rb_cWindow, "viewport",
      (VALUE(*)(...))rb_window_viewport, 0);
  rb_define_method(rb_cWindow, "viewport=",
      (VALUE(*)(...))rb_window_set_viewport, 1);
  rb_define_method(rb_cWindow, "active",
      (VALUE(*)(...))rb_window_active, 0);
  rb_define_method(rb_cWindow, "active=",
      (VALUE(*)(...))rb_window_set_active, 1);
  rb_define_method(rb_cWindow, "visible",
      (VALUE(*)(...))rb_window_visible, 0);
  rb_define_method(rb_cWindow, "visible=",
      (VALUE(*)(...))rb_window_set_visible, 1);
  rb_define_method(rb_cWindow, "arrows_visible",
      (VALUE(*)(...))rb_window_arrows_visible, 0);
  rb_define_method(rb_cWindow, "arrows_visible=",
      (VALUE(*)(...))rb_window_set_arrows_visible, 1);
  rb_define_method(rb_cWindow, "pause",
      (VALUE(*)(...))rb_window_pause, 0);
  rb_define_method(rb_cWindow, "pause=",
      (VALUE(*)(...))rb_window_set_pause, 1);
  rb_define_method(rb_cWindow, "x",
      (VALUE(*)(...))rb_window_x, 0);
  rb_define_method(rb_cWindow, "x=",
      (VALUE(*)(...))rb_window_set_x, 1);
  rb_define_method(rb_cWindow, "y",
      (VALUE(*)(...))rb_window_y, 0);
  rb_define_method(rb_cWindow, "y=",
      (VALUE(*)(...))rb_window_set_y, 1);
  rb_define_method(rb_cWindow, "width",
      (VALUE(*)(...))rb_window_width, 0);
  rb_define_method(rb_cWindow, "width=",
      (VALUE(*)(...))rb_window_set_width, 1);
  rb_define_method(rb_cWindow, "height",
      (VALUE(*)(...))rb_window_height, 0);
  rb_define_method(rb_cWindow, "height=",
      (VALUE(*)(...))rb_window_set_height, 1);
  rb_define_method(rb_cWindow, "z",
      (VALUE(*)(...))rb_window_z, 0);
  rb_define_method(rb_cWindow, "z=",
      (VALUE(*)(...))rb_window_set_z, 1);
  rb_define_method(rb_cWindow, "ox",
      (VALUE(*)(...))rb_window_ox, 0);
  rb_define_method(rb_cWindow, "ox=",
      (VALUE(*)(...))rb_window_set_ox, 1);
  rb_define_method(rb_cWindow, "oy",
      (VALUE(*)(...))rb_window_oy, 0);
  rb_define_method(rb_cWindow, "oy=",
      (VALUE(*)(...))rb_window_set_oy, 1);
  rb_define_method(rb_cWindow, "padding",
      (VALUE(*)(...))rb_window_padding, 0);
  rb_define_method(rb_cWindow, "padding=",
      (VALUE(*)(...))rb_window_set_padding, 1);
  rb_define_method(rb_cWindow, "padding_bottom",
      (VALUE(*)(...))rb_window_padding_bottom, 0);
  rb_define_method(rb_cWindow, "padding_bottom=",
      (VALUE(*)(...))rb_window_set_padding_bottom, 1);
  rb_define_method(rb_cWindow, "opacity",
      (VALUE(*)(...))rb_window_opacity, 0);
  rb_define_method(rb_cWindow, "opacity=",
      (VALUE(*)(...))rb_window_set_opacity, 1);
  rb_define_method(rb_cWindow, "back_opacity",
      (VALUE(*)(...))rb_window_back_opacity, 0);
  rb_define_method(rb_cWindow, "back_opacity=",
      (VALUE(*)(...))rb_window_set_back_opacity, 1);
  rb_define_method(rb_cWindow, "contents_opacity",
      (VALUE(*)(...))rb_window_contents_opacity, 0);
  rb_define_method(rb_cWindow, "contents_opacity=",
      (VALUE(*)(...))rb_window_set_contents_opacity, 1);
  rb_define_method(rb_cWindow, "openness",
      (VALUE(*)(...))rb_window_openness, 0);
  rb_define_method(rb_cWindow, "openness=",
      (VALUE(*)(...))rb_window_set_openness, 1);
  rb_define_method(rb_cWindow, "tone",
      (VALUE(*)(...))rb_window_tone, 0);
  rb_define_method(rb_cWindow, "tone=",
      (VALUE(*)(...))rb_window_set_tone, 1);
}

Window *convertWindow(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))window_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Window",
        rb_class2name(rb_obj_class(obj)));
  }
  Window *ret;
  Data_Get_Struct(obj, Window, ret);
  return ret;
}
Window *convertWindowOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertWindow(obj);
}
VALUE exportWindow(Window *ptr) {
  return ptr ? ptr->rb_parent : Qnil;
}

Window *Window::create(int x, int y, int width, int height) {
  VALUE ret = window_alloc(rb_cWindow);
  Window *ptr = convertWindow(ret);
  ptr->initialize(x, y, width, height);
  return ptr;
}
Window *Window::create() {
  VALUE ret = window_alloc(rb_cWindow);
  Window *ptr = convertWindow(ret);
  ptr->initialize();
  return ptr;
}

static void window_mark(Window *ptr) {
  if(ptr->windowskin) rb_gc_mark(ptr->windowskin->rb_parent);
  if(ptr->contents) rb_gc_mark(ptr->contents->rb_parent);
  rb_gc_mark(ptr->cursor_rect->rb_parent);
  if(ptr->viewport) rb_gc_mark(ptr->viewport->rb_parent);
  rb_gc_mark(ptr->tone->rb_parent);
}

static void window_free(Window *ptr) {
  ptr->dispose();
  xfree(ptr);
}

static VALUE window_alloc(VALUE klass) {
  Window *ptr = ALLOC(Window);
  ptr->windowskin = nullptr;
  ptr->contents = nullptr;
  ptr->cursor_rect = nullptr;
  ptr->viewport = nullptr;
  ptr->tone = nullptr;
  VALUE ret = Data_Wrap_Struct(klass, window_mark, window_free, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_window_initialize(int argc, VALUE *argv, VALUE self) {
  Window *ptr = convertWindow(self);
  switch(argc) {
    case 4:
      ptr->initialize(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]));
      break;
    case 0:
      ptr->initialize();
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0 or 4)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_window_initialize_copy(VALUE self, VALUE orig) {
  rb_raise(rb_eTypeError, "can't dup Window");
  return Qnil;
}
static VALUE rb_window_dup(VALUE self) {
  rb_raise(rb_eTypeError, "can't dup Window");
  return Qnil;
}
static VALUE rb_window_clone(VALUE self) {
  rb_raise(rb_eTypeError, "can't clone Window");
  return Qnil;
}

static VALUE rb_window_dispose(VALUE self) {
  Window *ptr = convertWindow(self);
  ptr->dispose();
  return Qnil;
}
static VALUE rb_window_disposed(VALUE self) {
  Window *ptr = convertWindow(self);
  return (ptr->disposed()) ? Qtrue : Qfalse;
}
static VALUE rb_window_update(VALUE self) {
  Window *ptr = convertWindow(self);
  ptr->update();
  return Qnil;
}
static VALUE rb_window_move(
    VALUE self, VALUE x, VALUE y, VALUE width, VALUE height) {
  Window *ptr = convertWindow(self);
  ptr->move(NUM2INT(x), NUM2INT(y), NUM2INT(width), NUM2INT(height));
  return Qnil;
}
static VALUE rb_window_open_p(VALUE self) {
  Window *ptr = convertWindow(self);
  return (ptr->open_p()) ? Qtrue : Qfalse;
}
static VALUE rb_window_close_p(VALUE self) {
  Window *ptr = convertWindow(self);
  return (ptr->close_p()) ? Qtrue : Qfalse;
}

static VALUE rb_window_windowskin(VALUE self) {
  Window *ptr = convertWindow(self);
  return exportBitmap(ptr->windowskin);
}
static VALUE rb_window_set_windowskin(VALUE self, VALUE windowskin) {
  Window *ptr = convertWindow(self);
  ptr->windowskin = convertBitmapOrNil(windowskin);
  return windowskin;
}
static VALUE rb_window_contents(VALUE self) {
  Window *ptr = convertWindow(self);
  return exportBitmap(ptr->contents);
}
static VALUE rb_window_set_contents(VALUE self, VALUE contents) {
  Window *ptr = convertWindow(self);
  ptr->contents = convertBitmapOrNil(contents);
  return contents;
}
static VALUE rb_window_cursor_rect(VALUE self) {
  Window *ptr = convertWindow(self);
  return exportRect(ptr->cursor_rect);
}
static VALUE rb_window_set_cursor_rect(VALUE self, VALUE cursor_rect) {
  Window *ptr = convertWindow(self);
  ptr->cursor_rect->set(convertRect(cursor_rect));
  return cursor_rect;
}
static VALUE rb_window_viewport(VALUE self) {
  Window *ptr = convertWindow(self);
  return exportViewport(ptr->viewport);
}
static VALUE rb_window_set_viewport(VALUE self, VALUE viewport) {
  Window *ptr = convertWindow(self);
  Graphics::unregister_renderable((Renderable*)ptr, ptr->viewport);
  ptr->viewport = convertViewportOrNil(viewport);
  Graphics::register_renderable((Renderable*)ptr, ptr->viewport);
  return viewport;
}
static VALUE rb_window_active(VALUE self) {
  Window *ptr = convertWindow(self);
  return (ptr->active) ? Qtrue : Qfalse;
}
static VALUE rb_window_set_active(VALUE self, VALUE active) {
  Window *ptr = convertWindow(self);
  ptr->active = RTEST(active);
  return active;
}
static VALUE rb_window_visible(VALUE self) {
  Window *ptr = convertWindow(self);
  return (ptr->visible) ? Qtrue : Qfalse;
}
static VALUE rb_window_set_visible(VALUE self, VALUE visible) {
  Window *ptr = convertWindow(self);
  ptr->visible = RTEST(visible);
  return visible;
}
static VALUE rb_window_arrows_visible(VALUE self) {
  Window *ptr = convertWindow(self);
  return (ptr->arrows_visible) ? Qtrue : Qfalse;
}
static VALUE rb_window_set_arrows_visible(VALUE self, VALUE arrows_visible) {
  Window *ptr = convertWindow(self);
  ptr->arrows_visible = RTEST(arrows_visible);
  return arrows_visible;
}
static VALUE rb_window_pause(VALUE self) {
  Window *ptr = convertWindow(self);
  return (ptr->pause) ? Qtrue : Qfalse;
}
static VALUE rb_window_set_pause(VALUE self, VALUE pause) {
  Window *ptr = convertWindow(self);
  ptr->pause = RTEST(pause);
  return pause;
}
static VALUE rb_window_x(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->x);
}
static VALUE rb_window_set_x(VALUE self, VALUE x) {
  Window *ptr = convertWindow(self);
  ptr->x = NUM2INT(x);
  return x;
}
static VALUE rb_window_y(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->y);
}
static VALUE rb_window_set_y(VALUE self, VALUE y) {
  Window *ptr = convertWindow(self);
  ptr->y = NUM2INT(y);
  return y;
}
static VALUE rb_window_width(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->width);
}
static VALUE rb_window_set_width(VALUE self, VALUE width) {
  Window *ptr = convertWindow(self);
  ptr->width = NUM2INT(width);
  return width;
}
static VALUE rb_window_height(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->height);
}
static VALUE rb_window_set_height(VALUE self, VALUE height) {
  Window *ptr = convertWindow(self);
  ptr->height = NUM2INT(height);
  return height;
}
static VALUE rb_window_z(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->z);
}
static VALUE rb_window_set_z(VALUE self, VALUE z) {
  Window *ptr = convertWindow(self);
  ptr->z = NUM2INT(z);
  ptr->renderable_entry.z = ptr->z;
  return z;
}
static VALUE rb_window_ox(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->ox);
}
static VALUE rb_window_set_ox(VALUE self, VALUE ox) {
  Window *ptr = convertWindow(self);
  ptr->ox = NUM2INT(ox);
  return ox;
}
static VALUE rb_window_oy(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->oy);
}
static VALUE rb_window_set_oy(VALUE self, VALUE oy) {
  Window *ptr = convertWindow(self);
  ptr->oy = NUM2INT(oy);
  return oy;
}
static VALUE rb_window_padding(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->padding);
}
static VALUE rb_window_set_padding(VALUE self, VALUE padding) {
  Window *ptr = convertWindow(self);
  ptr->padding = NUM2INT(padding);
  ptr->padding_bottom = ptr->padding;
  return padding;
}
static VALUE rb_window_padding_bottom(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->padding_bottom);
}
static VALUE rb_window_set_padding_bottom(VALUE self, VALUE padding_bottom) {
  Window *ptr = convertWindow(self);
  ptr->padding_bottom = NUM2INT(padding_bottom);
  return padding_bottom;
}
static VALUE rb_window_opacity(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->opacity);
}
static VALUE rb_window_set_opacity(VALUE self, VALUE opacity) {
  Window *ptr = convertWindow(self);
  ptr->opacity = saturate(NUM2INT(opacity), 0, 255);
  return opacity;
}
static VALUE rb_window_back_opacity(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->back_opacity);
}
static VALUE rb_window_set_back_opacity(VALUE self, VALUE back_opacity) {
  Window *ptr = convertWindow(self);
  ptr->back_opacity = saturate(NUM2INT(back_opacity), 0, 255);
  return back_opacity;
}
static VALUE rb_window_contents_opacity(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->contents_opacity);
}
static VALUE rb_window_set_contents_opacity(VALUE self, VALUE contents_opacity) {
  Window *ptr = convertWindow(self);
  ptr->contents_opacity = saturate(NUM2INT(contents_opacity), 0, 255);
  return contents_opacity;
}
static VALUE rb_window_openness(VALUE self) {
  Window *ptr = convertWindow(self);
  return INT2NUM(ptr->openness);
}
static VALUE rb_window_set_openness(VALUE self, VALUE openness) {
  Window *ptr = convertWindow(self);
  ptr->openness = saturate(NUM2INT(openness), 0, 255);
  return openness;
}
static VALUE rb_window_tone(VALUE self) {
  Window *ptr = convertWindow(self);
  return exportTone(ptr->tone);
}
static VALUE rb_window_set_tone(VALUE self, VALUE tone) {
  Window *ptr = convertWindow(self);
  ptr->tone->set(convertTone(tone));
  return tone;
}
