#include <string>

#include <SDL_image.h>

#include "Viewport.h"
#include "Graphics.h"
#include "misc.h"

void Viewport::initialize(int x, int y, int width, int height) {
  this->rect = Rect::create(x, y, width, height);
  this->visible = true;
  this->z = 0;
  this->ox = 0;
  this->oy = 0;
  this->color = Color::create();
  this->tone = Tone::create();

  this->is_disposed = false;
  this->renderable_entry.type = RenderableType::VIEWPORT;
  this->renderable_entry.y = 0;
  this->renderable_entry.z = this->z;
  this->renderable_entry.renderable_id = current_renderable_id++;
  this->renderables = new std::vector<Renderable*>();
  Graphics::register_renderable((Renderable*)this, nullptr);
}
void Viewport::initialize(Rect *rect) {
  initialize(rect->x, rect->y, rect->width, rect->height);
}
void Viewport::initialize() {
  // TODO
  initialize(0, 0, Graphics::width, Graphics::height);
}
void Viewport::dispose() {
  if(!this->is_disposed) {
    this->is_disposed = true;
    Graphics::unregister_renderable((Renderable*)this, nullptr);
  }
}
bool Viewport::disposed() {
  return this->is_disposed;
}
void Viewport::flash(Color *color, int duration) {
  fprintf(stderr, "TODO: Viewport::flash\n");
}
void Viewport::update() {
  // TODO: Viewport::update
}

void Viewport::render(
    SDL_Renderer *renderer,
    int rox, int roy, int rwidth, int rheight) {
  SDL_Rect clip_rect;
  clip_rect.x = rect->x;
  clip_rect.y = rect->y;
  clip_rect.w = rect->width;
  clip_rect.h = rect->height;
  SDL_RenderSetClipRect(renderer, &clip_rect);
  SDL_RenderSetViewport(renderer, &clip_rect);
  Graphics::sort_renderables(renderables);
  for(Renderable *r : *renderables) {
    Graphics::render_renderable(r, renderer, ox, oy, rect->width, rect->height);
  }
  SDL_RenderSetClipRect(renderer, NULL);
  SDL_RenderSetViewport(renderer, NULL);
}

static void viewport_mark(Viewport *);
static VALUE viewport_alloc(VALUE klass);

static VALUE rb_viewport_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_viewport_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_viewport_dup(VALUE self);
static VALUE rb_viewport_clone(VALUE self);

static VALUE rb_viewport_dispose(VALUE self);
static VALUE rb_viewport_disposed(VALUE self);
static VALUE rb_viewport_flash(VALUE self, VALUE color, VALUE duration);
static VALUE rb_viewport_update(VALUE self);
static VALUE rb_viewport_rect(VALUE self);
static VALUE rb_viewport_set_rect(VALUE self, VALUE rect);
static VALUE rb_viewport_visible(VALUE self);
static VALUE rb_viewport_set_visible(VALUE self, VALUE visible);
static VALUE rb_viewport_z(VALUE self);
static VALUE rb_viewport_set_z(VALUE self, VALUE z);
static VALUE rb_viewport_ox(VALUE self);
static VALUE rb_viewport_set_ox(VALUE self, VALUE ox);
static VALUE rb_viewport_oy(VALUE self);
static VALUE rb_viewport_set_oy(VALUE self, VALUE oy);
static VALUE rb_viewport_color(VALUE self);
static VALUE rb_viewport_set_color(VALUE self, VALUE tone);
static VALUE rb_viewport_tone(VALUE self);
static VALUE rb_viewport_set_tone(VALUE self, VALUE tone);

VALUE rb_cViewport;
void InitViewport() {
  rb_cViewport = rb_define_class("Viewport", rb_cObject);
  rb_define_alloc_func(rb_cViewport, viewport_alloc);
  rb_define_method(rb_cViewport, "initialize",
      (VALUE(*)(...))rb_viewport_initialize, -1);
  rb_define_method(rb_cViewport, "initialize_copy",
      (VALUE(*)(...))rb_viewport_initialize_copy, 1);

  rb_define_method(rb_cViewport, "dup",
      (VALUE(*)(...))rb_viewport_dup, 1);
  rb_define_method(rb_cViewport, "clone",
      (VALUE(*)(...))rb_viewport_clone, 1);

  rb_define_method(rb_cViewport, "dispose",
      (VALUE(*)(...))rb_viewport_dispose, 0);
  rb_define_method(rb_cViewport, "disposed?",
      (VALUE(*)(...))rb_viewport_disposed, 0);
  rb_define_method(rb_cViewport, "flash",
      (VALUE(*)(...))rb_viewport_flash, 2);
  rb_define_method(rb_cViewport, "update",
      (VALUE(*)(...))rb_viewport_update, 0);

  rb_define_method(rb_cViewport, "rect",
      (VALUE(*)(...))rb_viewport_rect, 0);
  rb_define_method(rb_cViewport, "rect=",
      (VALUE(*)(...))rb_viewport_set_rect, 1);
  rb_define_method(rb_cViewport, "visible",
      (VALUE(*)(...))rb_viewport_visible, 0);
  rb_define_method(rb_cViewport, "visible=",
      (VALUE(*)(...))rb_viewport_set_visible, 1);
  rb_define_method(rb_cViewport, "z",
      (VALUE(*)(...))rb_viewport_z, 0);
  rb_define_method(rb_cViewport, "z=",
      (VALUE(*)(...))rb_viewport_set_z, 1);
  rb_define_method(rb_cViewport, "ox",
      (VALUE(*)(...))rb_viewport_ox, 0);
  rb_define_method(rb_cViewport, "ox=",
      (VALUE(*)(...))rb_viewport_set_ox, 1);
  rb_define_method(rb_cViewport, "oy",
      (VALUE(*)(...))rb_viewport_oy, 0);
  rb_define_method(rb_cViewport, "oy=",
      (VALUE(*)(...))rb_viewport_set_oy, 1);
  rb_define_method(rb_cViewport, "color",
      (VALUE(*)(...))rb_viewport_color, 0);
  rb_define_method(rb_cViewport, "color=",
      (VALUE(*)(...))rb_viewport_set_color, 1);
  rb_define_method(rb_cViewport, "tone",
      (VALUE(*)(...))rb_viewport_tone, 0);
  rb_define_method(rb_cViewport, "tone=",
      (VALUE(*)(...))rb_viewport_set_tone, 1);
}

Viewport *convertViewport(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))viewport_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Viewport",
        rb_class2name(rb_obj_class(obj)));
  }
  Viewport *ret;
  Data_Get_Struct(obj, Viewport, ret);
  return ret;
}
Viewport *convertViewportOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertViewport(obj);
}
VALUE exportViewport(Viewport *ptr) {
  return ptr ? ptr->rb_parent : Qnil;
}

Viewport *Viewport::create(int x, int y, int width, int height) {
  VALUE ret = viewport_alloc(rb_cViewport);
  Viewport *ptr = convertViewport(ret);
  ptr->initialize(x, y, width, height);
  return ptr;
}
Viewport *Viewport::create(Rect *rect) {
  VALUE ret = viewport_alloc(rb_cViewport);
  Viewport *ptr = convertViewport(ret);
  ptr->initialize(rect);
  return ptr;
}
Viewport *Viewport::create() {
  VALUE ret = viewport_alloc(rb_cViewport);
  Viewport *ptr = convertViewport(ret);
  ptr->initialize();
  return ptr;
}

static void viewport_mark(Viewport *ptr) {
  rb_gc_mark(ptr->rect->rb_parent);
  rb_gc_mark(ptr->color->rb_parent);
  rb_gc_mark(ptr->tone->rb_parent);
}

static void viewport_free(Viewport *ptr) {
  ptr->dispose();
  xfree(ptr);
}

static VALUE viewport_alloc(VALUE klass) {
  Viewport *ptr = ALLOC(Viewport);
  VALUE ret = Data_Wrap_Struct(klass, viewport_mark, viewport_free, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_viewport_initialize(int argc, VALUE *argv, VALUE self) {
  Viewport *ptr = convertViewport(self);
  switch(argc) {
    case 4:
      ptr->initialize(
          NUM2INT(argv[0]), NUM2INT(argv[1]), NUM2INT(argv[2]),
          NUM2INT(argv[3]));
      break;
    case 1:
      ptr->initialize(convertRect(argv[0]));
      break;
    case 0:
      ptr->initialize();
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0 or 1 or 4)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_viewport_initialize_copy(VALUE self, VALUE orig) {
  rb_raise(rb_eTypeError, "can't dup Viewport");
  return Qnil;
}
static VALUE rb_viewport_dup(VALUE self) {
  rb_raise(rb_eTypeError, "can't dup Viewport");
  return Qnil;
}
static VALUE rb_viewport_clone(VALUE self) {
  rb_raise(rb_eTypeError, "can't clone Viewport");
  return Qnil;
}

static VALUE rb_viewport_dispose(VALUE self) {
  Viewport *ptr = convertViewport(self);
  ptr->dispose();
  return Qnil;
}
static VALUE rb_viewport_disposed(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return (ptr->disposed()) ? Qtrue : Qfalse;
}
static VALUE rb_viewport_flash(VALUE self, VALUE color, VALUE duration) {
  Viewport *ptr = convertViewport(self);
  ptr->flash(convertColor(color), NUM2INT(duration));
  return Qnil;
}
static VALUE rb_viewport_update(VALUE self) {
  Viewport *ptr = convertViewport(self);
  ptr->update();
  return Qnil;
}

static VALUE rb_viewport_rect(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return exportRect(ptr->rect);
}
static VALUE rb_viewport_set_rect(VALUE self, VALUE rect) {
  Viewport *ptr = convertViewport(self);
  ptr->rect->set(convertRect(rect));
  return rect;
}
static VALUE rb_viewport_visible(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return (ptr->visible) ? Qtrue : Qfalse;
}
static VALUE rb_viewport_set_visible(VALUE self, VALUE visible) {
  Viewport *ptr = convertViewport(self);
  ptr->visible = RTEST(visible);
  return visible;
}
static VALUE rb_viewport_z(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return INT2NUM(ptr->z);
}
static VALUE rb_viewport_set_z(VALUE self, VALUE z) {
  Viewport *ptr = convertViewport(self);
  ptr->z = NUM2INT(z);
  ptr->renderable_entry.z = ptr->z;
  return z;
}
static VALUE rb_viewport_ox(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return INT2NUM(ptr->ox);
}
static VALUE rb_viewport_set_ox(VALUE self, VALUE ox) {
  Viewport *ptr = convertViewport(self);
  ptr->ox = NUM2INT(ox);
  return ox;
}
static VALUE rb_viewport_oy(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return INT2NUM(ptr->oy);
}
static VALUE rb_viewport_set_oy(VALUE self, VALUE oy) {
  Viewport *ptr = convertViewport(self);
  ptr->oy = NUM2INT(oy);
  return oy;
}
static VALUE rb_viewport_color(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return exportColor(ptr->color);
}
static VALUE rb_viewport_set_color(VALUE self, VALUE color) {
  Viewport *ptr = convertViewport(self);
  ptr->color->set(convertColor(color));
  return color;
}
static VALUE rb_viewport_tone(VALUE self) {
  Viewport *ptr = convertViewport(self);
  return exportTone(ptr->tone);
}
static VALUE rb_viewport_set_tone(VALUE self, VALUE tone) {
  Viewport *ptr = convertViewport(self);
  ptr->tone->set(convertTone(tone));
  return tone;
}
