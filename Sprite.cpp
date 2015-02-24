#include <string>

#include <SDL_image.h>

#include "Sprite.h"
#include "misc.h"

void Sprite::initialize(Viewport *viewport) {
  this->bitmap = nullptr;
  this->src_rect = Rect::create();
  this->viewport = viewport;
  this->visible = true;
  this->x = 0;
  this->y = 0;
  this->z = 0;
  this->ox = 0;
  this->oy = 0;
  this->zoom_x = 1.0;
  this->zoom_y = 1.0;
  this->angle = 0.0;
  this->wave_amp = 0;
  this->wave_length = 180;
  this->wave_speed = 360;
  this->wave_phase = 0.0;
  this->mirror = false;
  this->bush_depth = 0;
  this->bush_opacity = 128;
  this->blend_type = 0;
  this->tone = Tone::create();

  this->is_disposed = false;
}
void Sprite::dispose() {
  if(!this->is_disposed) {
    this->is_disposed = true;
  }
}
bool Sprite::disposed() {
  return this->is_disposed;
}
void Sprite::flash(Color *color, int duration) {
  fprintf(stderr, "TODO: Sprite::flash\n");
}
void Sprite::update() {
  // TODO: Sprite::update
}
int Sprite::width() {
  return this->src_rect->width;
}
int Sprite::height() {
  return this->src_rect->height;
}

static void sprite_mark(Sprite *);
static VALUE sprite_alloc(VALUE klass);

static VALUE rb_sprite_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_sprite_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_sprite_dup(VALUE self);
static VALUE rb_sprite_clone(VALUE self);

static VALUE rb_sprite_dispose(VALUE self);
static VALUE rb_sprite_disposed(VALUE self);
static VALUE rb_sprite_flash(VALUE self, VALUE color, VALUE duration);
static VALUE rb_sprite_update(VALUE self);
static VALUE rb_sprite_width(VALUE self);
static VALUE rb_sprite_height(VALUE self);
static VALUE rb_sprite_bitmap(VALUE self);
static VALUE rb_sprite_set_bitmap(VALUE self, VALUE bitmap);
static VALUE rb_sprite_src_rect(VALUE self);
static VALUE rb_sprite_set_src_rect(VALUE self, VALUE src_rect);
static VALUE rb_sprite_viewport(VALUE self);
static VALUE rb_sprite_set_viewport(VALUE self, VALUE viewport);
static VALUE rb_sprite_visible(VALUE self);
static VALUE rb_sprite_set_visible(VALUE self, VALUE visible);
static VALUE rb_sprite_x(VALUE self);
static VALUE rb_sprite_set_x(VALUE self, VALUE x);
static VALUE rb_sprite_y(VALUE self);
static VALUE rb_sprite_set_y(VALUE self, VALUE y);
static VALUE rb_sprite_z(VALUE self);
static VALUE rb_sprite_set_z(VALUE self, VALUE z);
static VALUE rb_sprite_ox(VALUE self);
static VALUE rb_sprite_set_ox(VALUE self, VALUE ox);
static VALUE rb_sprite_oy(VALUE self);
static VALUE rb_sprite_set_oy(VALUE self, VALUE oy);
static VALUE rb_sprite_zoom_x(VALUE self);
static VALUE rb_sprite_set_zoom_x(VALUE self, VALUE zoom_x);
static VALUE rb_sprite_zoom_y(VALUE self);
static VALUE rb_sprite_set_zoom_y(VALUE self, VALUE zoom_y);
static VALUE rb_sprite_angle(VALUE self);
static VALUE rb_sprite_set_angle(VALUE self, VALUE angle);
static VALUE rb_sprite_wave_amp(VALUE self);
static VALUE rb_sprite_set_wave_amp(VALUE self, VALUE wave_amp);
static VALUE rb_sprite_wave_length(VALUE self);
static VALUE rb_sprite_set_wave_length(VALUE self, VALUE wave_length);
static VALUE rb_sprite_wave_speed(VALUE self);
static VALUE rb_sprite_set_wave_speed(VALUE self, VALUE wave_speed);
static VALUE rb_sprite_wave_phase(VALUE self);
static VALUE rb_sprite_set_wave_phase(VALUE self, VALUE wave_phase);
static VALUE rb_sprite_mirror(VALUE self);
static VALUE rb_sprite_set_mirror(VALUE self, VALUE mirror);
static VALUE rb_sprite_bush_depth(VALUE self);
static VALUE rb_sprite_set_bush_depth(VALUE self, VALUE bush_depth);
static VALUE rb_sprite_bush_opacity(VALUE self);
static VALUE rb_sprite_set_bush_opacity(VALUE self, VALUE bush_opacity);
static VALUE rb_sprite_opacity(VALUE self);
static VALUE rb_sprite_set_opacity(VALUE self, VALUE opacity);
static VALUE rb_sprite_blend_type(VALUE self);
static VALUE rb_sprite_set_blend_type(VALUE self, VALUE blend_type);
static VALUE rb_sprite_tone(VALUE self);
static VALUE rb_sprite_set_tone(VALUE self, VALUE tone);

VALUE rb_cSprite;
void InitSprite() {
  rb_cSprite = rb_define_class("Sprite", rb_cObject);
  rb_define_alloc_func(rb_cSprite, sprite_alloc);
  rb_define_method(rb_cSprite, "initialize",
      (VALUE(*)(...))rb_sprite_initialize, -1);
  rb_define_method(rb_cSprite, "initialize_copy",
      (VALUE(*)(...))rb_sprite_initialize_copy, 1);

  rb_define_method(rb_cSprite, "dup",
      (VALUE(*)(...))rb_sprite_dup, 1);
  rb_define_method(rb_cSprite, "clone",
      (VALUE(*)(...))rb_sprite_clone, 1);

  rb_define_method(rb_cSprite, "dispose",
      (VALUE(*)(...))rb_sprite_dispose, 0);
  rb_define_method(rb_cSprite, "disposed?",
      (VALUE(*)(...))rb_sprite_disposed, 0);
  rb_define_method(rb_cSprite, "flash",
      (VALUE(*)(...))rb_sprite_flash, 2);
  rb_define_method(rb_cSprite, "update",
      (VALUE(*)(...))rb_sprite_update, 0);
  rb_define_method(rb_cSprite, "width",
      (VALUE(*)(...))rb_sprite_width, 0);
  rb_define_method(rb_cSprite, "height",
      (VALUE(*)(...))rb_sprite_height, 0);

  rb_define_method(rb_cSprite, "bitmap",
      (VALUE(*)(...))rb_sprite_bitmap, 0);
  rb_define_method(rb_cSprite, "bitmap=",
      (VALUE(*)(...))rb_sprite_set_bitmap, 1);
  rb_define_method(rb_cSprite, "src_rect",
      (VALUE(*)(...))rb_sprite_src_rect, 0);
  rb_define_method(rb_cSprite, "src_rect=",
      (VALUE(*)(...))rb_sprite_set_src_rect, 1);
  rb_define_method(rb_cSprite, "viewport",
      (VALUE(*)(...))rb_sprite_viewport, 0);
  rb_define_method(rb_cSprite, "viewport=",
      (VALUE(*)(...))rb_sprite_set_viewport, 1);
  rb_define_method(rb_cSprite, "visible",
      (VALUE(*)(...))rb_sprite_visible, 0);
  rb_define_method(rb_cSprite, "visible=",
      (VALUE(*)(...))rb_sprite_set_visible, 1);
  rb_define_method(rb_cSprite, "x",
      (VALUE(*)(...))rb_sprite_x, 0);
  rb_define_method(rb_cSprite, "x=",
      (VALUE(*)(...))rb_sprite_set_x, 1);
  rb_define_method(rb_cSprite, "y",
      (VALUE(*)(...))rb_sprite_y, 0);
  rb_define_method(rb_cSprite, "y=",
      (VALUE(*)(...))rb_sprite_set_y, 1);
  rb_define_method(rb_cSprite, "z",
      (VALUE(*)(...))rb_sprite_z, 0);
  rb_define_method(rb_cSprite, "z=",
      (VALUE(*)(...))rb_sprite_set_z, 1);
  rb_define_method(rb_cSprite, "ox",
      (VALUE(*)(...))rb_sprite_ox, 0);
  rb_define_method(rb_cSprite, "ox=",
      (VALUE(*)(...))rb_sprite_set_ox, 1);
  rb_define_method(rb_cSprite, "oy",
      (VALUE(*)(...))rb_sprite_oy, 0);
  rb_define_method(rb_cSprite, "oy=",
      (VALUE(*)(...))rb_sprite_set_oy, 1);
  rb_define_method(rb_cSprite, "zoom_x",
      (VALUE(*)(...))rb_sprite_zoom_x, 0);
  rb_define_method(rb_cSprite, "zoom_x=",
      (VALUE(*)(...))rb_sprite_set_zoom_x, 1);
  rb_define_method(rb_cSprite, "zoom_y",
      (VALUE(*)(...))rb_sprite_zoom_y, 0);
  rb_define_method(rb_cSprite, "zoom_y=",
      (VALUE(*)(...))rb_sprite_set_zoom_y, 1);
  rb_define_method(rb_cSprite, "angle",
      (VALUE(*)(...))rb_sprite_angle, 0);
  rb_define_method(rb_cSprite, "angle=",
      (VALUE(*)(...))rb_sprite_set_angle, 1);
  rb_define_method(rb_cSprite, "wave_amp",
      (VALUE(*)(...))rb_sprite_wave_amp, 0);
  rb_define_method(rb_cSprite, "wave_amp=",
      (VALUE(*)(...))rb_sprite_set_wave_amp, 1);
  rb_define_method(rb_cSprite, "wave_length",
      (VALUE(*)(...))rb_sprite_wave_length, 0);
  rb_define_method(rb_cSprite, "wave_length=",
      (VALUE(*)(...))rb_sprite_set_wave_length, 1);
  rb_define_method(rb_cSprite, "wave_speed",
      (VALUE(*)(...))rb_sprite_wave_speed, 0);
  rb_define_method(rb_cSprite, "wave_speed=",
      (VALUE(*)(...))rb_sprite_set_wave_speed, 1);
  rb_define_method(rb_cSprite, "wave_phase",
      (VALUE(*)(...))rb_sprite_wave_phase, 0);
  rb_define_method(rb_cSprite, "wave_phase=",
      (VALUE(*)(...))rb_sprite_set_wave_phase, 1);
  rb_define_method(rb_cSprite, "mirror",
      (VALUE(*)(...))rb_sprite_mirror, 0);
  rb_define_method(rb_cSprite, "mirror=",
      (VALUE(*)(...))rb_sprite_set_mirror, 1);
  rb_define_method(rb_cSprite, "bush_depth",
      (VALUE(*)(...))rb_sprite_bush_depth, 0);
  rb_define_method(rb_cSprite, "bush_depth=",
      (VALUE(*)(...))rb_sprite_set_bush_depth, 1);
  rb_define_method(rb_cSprite, "bush_opacity",
      (VALUE(*)(...))rb_sprite_bush_opacity, 0);
  rb_define_method(rb_cSprite, "bush_opacity=",
      (VALUE(*)(...))rb_sprite_set_bush_opacity, 1);
  rb_define_method(rb_cSprite, "opacity",
      (VALUE(*)(...))rb_sprite_opacity, 0);
  rb_define_method(rb_cSprite, "opacity=",
      (VALUE(*)(...))rb_sprite_set_opacity, 1);
  rb_define_method(rb_cSprite, "blend_type",
      (VALUE(*)(...))rb_sprite_blend_type, 0);
  rb_define_method(rb_cSprite, "blend_type=",
      (VALUE(*)(...))rb_sprite_set_blend_type, 1);
  rb_define_method(rb_cSprite, "tone",
      (VALUE(*)(...))rb_sprite_tone, 0);
  rb_define_method(rb_cSprite, "tone=",
      (VALUE(*)(...))rb_sprite_set_tone, 1);
}

Sprite *convertSprite(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))sprite_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Sprite",
        rb_class2name(rb_obj_class(obj)));
  }
  Sprite *ret;
  Data_Get_Struct(obj, Sprite, ret);
  return ret;
}
Sprite *convertSpriteOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertSprite(obj);
}
VALUE exportSprite(Sprite *ptr) {
  return ptr ? ptr->rb_parent : Qnil;
}

Sprite *Sprite::create(Viewport *viewport) {
  VALUE ret = sprite_alloc(rb_cSprite);
  Sprite *ptr = convertSprite(ret);
  ptr->initialize(viewport);
  return ptr;
}

static void sprite_mark(Sprite *ptr) {
  if(ptr->bitmap) rb_gc_mark(ptr->bitmap->rb_parent);
  rb_gc_mark(ptr->src_rect->rb_parent);
  if(ptr->viewport) rb_gc_mark(ptr->viewport->rb_parent);
  rb_gc_mark(ptr->tone->rb_parent);
}

static void sprite_free(Sprite *ptr) {
  ptr->dispose();
  xfree(ptr);
}

static VALUE sprite_alloc(VALUE klass) {
  Sprite *ptr = ALLOC(Sprite);
  VALUE ret = Data_Wrap_Struct(klass, sprite_mark, sprite_free, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_sprite_initialize(int argc, VALUE *argv, VALUE self) {
  Sprite *ptr = convertSprite(self);
  switch(argc) {
    case 1:
      ptr->initialize(convertViewport(argv[0]));
      break;
    case 0:
      ptr->initialize();
      break;
    default:
      rb_raise(rb_eArgError,
          "wrong number of arguments (%d for 0..1)", argc);
      break;
  }
  return Qnil;
}
static VALUE rb_sprite_initialize_copy(VALUE self, VALUE orig) {
  rb_raise(rb_eTypeError, "can't dup Sprite");
  return Qnil;
}
static VALUE rb_sprite_dup(VALUE self) {
  rb_raise(rb_eTypeError, "can't dup Sprite");
  return Qnil;
}
static VALUE rb_sprite_clone(VALUE self) {
  rb_raise(rb_eTypeError, "can't clone Sprite");
  return Qnil;
}

static VALUE rb_sprite_dispose(VALUE self) {
  Sprite *ptr = convertSprite(self);
  ptr->dispose();
  return Qnil;
}
static VALUE rb_sprite_disposed(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return (ptr->disposed()) ? Qtrue : Qfalse;
}
static VALUE rb_sprite_flash(VALUE self, VALUE color, VALUE duration) {
  Sprite *ptr = convertSprite(self);
  ptr->flash(convertColor(color), NUM2INT(duration));
  return Qnil;
}
static VALUE rb_sprite_update(VALUE self) {
  Sprite *ptr = convertSprite(self);
  ptr->update();
  return Qnil;
}
static VALUE rb_sprite_width(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->width());
}
static VALUE rb_sprite_height(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->height());
}

static VALUE rb_sprite_bitmap(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return exportBitmap(ptr->bitmap);
}
static VALUE rb_sprite_set_bitmap(VALUE self, VALUE bitmap) {
  Sprite *ptr = convertSprite(self);
  ptr->bitmap = convertBitmapOrNil(bitmap);
  return bitmap;
}
static VALUE rb_sprite_src_rect(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return exportRect(ptr->src_rect);
}
static VALUE rb_sprite_set_src_rect(VALUE self, VALUE src_rect) {
  Sprite *ptr = convertSprite(self);
  ptr->src_rect->set(convertRect(src_rect));
  return src_rect;
}
static VALUE rb_sprite_viewport(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return exportViewport(ptr->viewport);
}
static VALUE rb_sprite_set_viewport(VALUE self, VALUE viewport) {
  Sprite *ptr = convertSprite(self);
  ptr->viewport = convertViewportOrNil(viewport);
  return viewport;
}
static VALUE rb_sprite_visible(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return (ptr->visible) ? Qtrue : Qfalse;
}
static VALUE rb_sprite_set_visible(VALUE self, VALUE visible) {
  Sprite *ptr = convertSprite(self);
  ptr->visible = RTEST(visible);
  return visible;
}
static VALUE rb_sprite_x(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->x);
}
static VALUE rb_sprite_set_x(VALUE self, VALUE x) {
  Sprite *ptr = convertSprite(self);
  ptr->x = NUM2INT(x);
  return x;
}
static VALUE rb_sprite_y(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->y);
}
static VALUE rb_sprite_set_y(VALUE self, VALUE y) {
  Sprite *ptr = convertSprite(self);
  ptr->y = NUM2INT(y);
  return y;
}
static VALUE rb_sprite_z(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->z);
}
static VALUE rb_sprite_set_z(VALUE self, VALUE z) {
  Sprite *ptr = convertSprite(self);
  ptr->z = NUM2INT(z);
  return z;
}
static VALUE rb_sprite_ox(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->ox);
}
static VALUE rb_sprite_set_ox(VALUE self, VALUE ox) {
  Sprite *ptr = convertSprite(self);
  ptr->ox = NUM2INT(ox);
  return ox;
}
static VALUE rb_sprite_oy(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->oy);
}
static VALUE rb_sprite_set_oy(VALUE self, VALUE oy) {
  Sprite *ptr = convertSprite(self);
  ptr->oy = NUM2INT(oy);
  return oy;
}
static VALUE rb_sprite_zoom_x(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return DBL2NUM(ptr->zoom_x);
}
static VALUE rb_sprite_set_zoom_x(VALUE self, VALUE zoom_x) {
  Sprite *ptr = convertSprite(self);
  ptr->zoom_x = NUM2DBL(zoom_x);
  return zoom_x;
}
static VALUE rb_sprite_zoom_y(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return DBL2NUM(ptr->zoom_y);
}
static VALUE rb_sprite_set_zoom_y(VALUE self, VALUE zoom_y) {
  Sprite *ptr = convertSprite(self);
  ptr->zoom_y = NUM2DBL(zoom_y);
  return zoom_y;
}
static VALUE rb_sprite_angle(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return DBL2NUM(ptr->angle);
}
static VALUE rb_sprite_set_angle(VALUE self, VALUE angle) {
  Sprite *ptr = convertSprite(self);
  ptr->angle = NUM2DBL(angle);
  return angle;
}
static VALUE rb_sprite_wave_amp(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->wave_amp);
}
static VALUE rb_sprite_set_wave_amp(VALUE self, VALUE wave_amp) {
  Sprite *ptr = convertSprite(self);
  ptr->wave_amp = NUM2INT(wave_amp);
  return wave_amp;
}
static VALUE rb_sprite_wave_length(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->wave_length);
}
static VALUE rb_sprite_set_wave_length(VALUE self, VALUE wave_length) {
  Sprite *ptr = convertSprite(self);
  ptr->wave_length = NUM2INT(wave_length);
  return wave_length;
}
static VALUE rb_sprite_wave_speed(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->wave_speed);
}
static VALUE rb_sprite_set_wave_speed(VALUE self, VALUE wave_speed) {
  Sprite *ptr = convertSprite(self);
  ptr->wave_speed = NUM2INT(wave_speed);
  return wave_speed;
}
static VALUE rb_sprite_wave_phase(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return DBL2NUM(ptr->wave_phase);
}
static VALUE rb_sprite_set_wave_phase(VALUE self, VALUE wave_phase) {
  Sprite *ptr = convertSprite(self);
  ptr->wave_phase = NUM2DBL(wave_phase);
  return wave_phase;
}
static VALUE rb_sprite_mirror(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return (ptr->mirror) ? Qtrue : Qfalse;
}
static VALUE rb_sprite_set_mirror(VALUE self, VALUE mirror) {
  Sprite *ptr = convertSprite(self);
  ptr->mirror = RTEST(mirror);
  return mirror;
}
static VALUE rb_sprite_bush_depth(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->bush_depth);
}
static VALUE rb_sprite_set_bush_depth(VALUE self, VALUE bush_depth) {
  Sprite *ptr = convertSprite(self);
  ptr->bush_depth = NUM2INT(bush_depth);
  return bush_depth;
}
static VALUE rb_sprite_bush_opacity(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->bush_opacity);
}
static VALUE rb_sprite_set_bush_opacity(VALUE self, VALUE bush_opacity) {
  Sprite *ptr = convertSprite(self);
  ptr->bush_opacity = NUM2INT(bush_opacity);
  return bush_opacity;
}
static VALUE rb_sprite_opacity(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->opacity);
}
static VALUE rb_sprite_set_opacity(VALUE self, VALUE opacity) {
  Sprite *ptr = convertSprite(self);
  ptr->opacity = NUM2INT(opacity);
  return opacity;
}
static VALUE rb_sprite_blend_type(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return INT2NUM(ptr->blend_type);
}
static VALUE rb_sprite_set_blend_type(VALUE self, VALUE blend_type) {
  Sprite *ptr = convertSprite(self);
  ptr->blend_type = NUM2INT(blend_type);
  return blend_type;
}
static VALUE rb_sprite_tone(VALUE self) {
  Sprite *ptr = convertSprite(self);
  return exportTone(ptr->tone);
}
static VALUE rb_sprite_set_tone(VALUE self, VALUE tone) {
  Sprite *ptr = convertSprite(self);
  ptr->tone->set(convertTone(tone));
  return tone;
}
