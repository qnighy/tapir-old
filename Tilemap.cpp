#include <string>

#include <SDL_image.h>

#include "Tilemap.h"
#include "Graphics.h"
#include "misc.h"

static VALUE bitmaparray_alloc(VALUE klass);

void Tilemap::initialize(Viewport *viewport) {
  for(int i = 0; i < 9; ++i) this->bitmaps[i] = nullptr;
  this->bitmap_array = bitmaparray_alloc(rb_cBitmapArray);
  convertBitmapArray(this->bitmap_array)->tilemap = this;
  this->map_data = nullptr;
  this->flash_data = nullptr;
  this->flags = nullptr;
  this->viewport = viewport;
  this->visible = true;
  this->ox = 0;
  this->oy = 0;

  this->is_disposed = false;
  this->renderable_entry.type = RenderableType::TILEMAP;
  this->renderable_entry.y = 0;
  this->renderable_entry.z = 0;
  this->renderable_entry.renderable_id = current_renderable_id++;
  Graphics::register_renderable((Renderable*)this, this->viewport);

  for(int i = 0; i < 3; ++i) {
    for(int j = 0; j < 5; ++j) {
      textures[i][j] = nullptr;
    }
  }
}
void Tilemap::dispose() {
  if(!this->is_disposed) {
    Graphics::unregister_renderable((Renderable*)this, this->viewport);
    for(int i = 0; i < 3; ++i) {
      for(int j = 0; j < 5; ++j) {
        if(textures[i][j]) {
          SDL_DestroyTexture(textures[i][j]);
        }
      }
    }
    this->is_disposed = true;
  }
}
bool Tilemap::disposed() {
  return this->is_disposed;
}
void Tilemap::update() {
  // TODO: Tilemap::update
}

void Tilemap::render(SDL_Renderer *renderer) {
  if(!this->visible || !this->map_data) return;
  int ysize = this->map_data->ysize;
  int xsize = this->map_data->xsize;
  if(ysize == 0 || xsize == 0) return;
  int syi = oy/32;
  int sxi = ox/32;
  int eyi = (oy+Graphics::height)/32+1;
  int exi = (ox+Graphics::width)/32+1;
  if(textures[0][0]) {
    int w, h;
    SDL_QueryTexture(textures[0][0], NULL, NULL, &w, &h);
    if(w != xsize*32 || h != ysize*32) {
      for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 5; ++j) {
          SDL_DestroyTexture(textures[i][j]);
          textures[i][j] = nullptr;
        }
      }
    }
  }
  bool updated = false;
  if(!textures[0][0]) {
    updated = true;
    for(int i = 0; i < 3; ++i) {
      for(int j = 0; j < 5; ++j) {
        textures[i][j] =
          SDL_CreateTexture(
              renderer, SDL_PIXELFORMAT_ABGR8888,
              SDL_TEXTUREACCESS_STREAMING, xsize*32, ysize*32);
        SDL_SetTextureAlphaMod(textures[i][j], 255);
        SDL_SetTextureBlendMode(textures[i][j], SDL_BLENDMODE_BLEND);
      }
    }
  }
  // return;
  // fprintf(stderr, "render!\n");
  // TODO: Tilemap::render
  for(int i = 0; i < 3; ++i) {
    if(!updated) break;
    for(int j = 0; j < 5; ++j) {
      void *pixels;
      int pitch;
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
      SDL_LockTexture(textures[i][j], NULL, &pixels, &pitch);
      SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
          pixels, xsize*32, ysize*32, 32, pitch,
          rmask, gmask, bmask, amask);
      int zi = j == 4 ? 2 : j;
      for(int yi = 0; yi < ysize; ++yi) {
        for(int xi = 0; xi < xsize; ++xi) {
          int tileid = this->map_data->get(xi, yi, zi);
          if(j == 3) {
            SDL_Rect dst_rect;
            dst_rect = {xi*32, yi*32, 16, 16};
            SDL_FillRect(surface, &dst_rect,
                SDL_MapRGBA(surface->format, 0, 0, 0,
                  (tileid&1) ? 128 : 0));
            dst_rect = {xi*32+16, yi*32, 16, 16};
            SDL_FillRect(surface, &dst_rect,
                SDL_MapRGBA(surface->format, 0, 0, 0,
                  (tileid&2) ? 128 : 0));
            dst_rect = {xi*32, yi*32+16, 16, 16};
            SDL_FillRect(surface, &dst_rect,
                SDL_MapRGBA(surface->format, 0, 0, 0,
                  (tileid&4) ? 128 : 0));
            dst_rect = {xi*32+16, yi*32+16, 16, 16};
            SDL_FillRect(surface, &dst_rect,
                SDL_MapRGBA(surface->format, 0, 0, 0,
                  (tileid&8) ? 128 : 0));
            continue;
          }
          int flag = this->flags ? this->flags->get(tileid) : 0;
          if((flag&0x0010) && j==2) continue;
          if((!(flag&0x0010)) && j==4) continue;
          if(0x0000 < tileid && tileid < 0x0400) {
            int bitmapid = 5 + ((tileid>>8)&3);
            SDL_Rect src_rect = {0, 0, 32, 32};
            src_rect.x = ((tileid&7)|((tileid>>4)&8))*32;
            src_rect.y = ((tileid>>3)&15)*32;
            SDL_Rect dst_rect = {xi*32, yi*32, 32, 32};
            if(!this->bitmaps[bitmapid]) continue;
            SDL_SetSurfaceBlendMode(
                this->bitmaps[bitmapid]->surface, SDL_BLENDMODE_NONE);
            SDL_BlitSurface(
                this->bitmaps[bitmapid]->surface, &src_rect,
                surface, &dst_rect);
          } else {
            SDL_Rect dst_rect = {xi*32, yi*32, 32, 32};
            SDL_FillRect(surface, &dst_rect,
                SDL_MapRGBA(surface->format, 0, 0, 0, 0));
          }
        }
      }
      SDL_FreeSurface(surface);
      SDL_UnlockTexture(textures[i][j]);
    }
  }
  for(int j = 0; j < 5; ++j) {
    SDL_Rect dst_rect;
    dst_rect.x = -ox;
    dst_rect.y = -oy;
    dst_rect.w = xsize*32;
    dst_rect.h = ysize*32;
    SDL_RenderCopy(renderer, textures[0][j], NULL, &dst_rect);
  }
}

static void tilemap_mark(Tilemap *);
static VALUE tilemap_alloc(VALUE klass);

static VALUE rb_tilemap_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_tilemap_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_tilemap_dup(VALUE self);
static VALUE rb_tilemap_clone(VALUE self);

static VALUE rb_tilemap_dispose(VALUE self);
static VALUE rb_tilemap_disposed(VALUE self);
static VALUE rb_tilemap_update(VALUE self);
static VALUE rb_tilemap_bitmaps(VALUE self);
static VALUE rb_tilemap_map_data(VALUE self);
static VALUE rb_tilemap_set_map_data(VALUE self, VALUE map_data);
static VALUE rb_tilemap_flash_data(VALUE self);
static VALUE rb_tilemap_set_flash_data(VALUE self, VALUE flash_data);
static VALUE rb_tilemap_flags(VALUE self);
static VALUE rb_tilemap_set_flags(VALUE self, VALUE flags);
static VALUE rb_tilemap_viewport(VALUE self);
static VALUE rb_tilemap_set_viewport(VALUE self, VALUE viewport);
static VALUE rb_tilemap_visible(VALUE self);
static VALUE rb_tilemap_set_visible(VALUE self, VALUE visible);
static VALUE rb_tilemap_ox(VALUE self);
static VALUE rb_tilemap_set_ox(VALUE self, VALUE ox);
static VALUE rb_tilemap_oy(VALUE self);
static VALUE rb_tilemap_set_oy(VALUE self, VALUE oy);

static void bitmaparray_mark(BitmapArray *);
static VALUE bitmaparray_alloc(VALUE klass);

static VALUE rb_bitmaparray_get(VALUE self, VALUE index);
static VALUE rb_bitmaparray_set(VALUE self, VALUE index, VALUE val);

VALUE rb_cTilemap;
VALUE rb_cBitmapArray;
void InitTilemap() {
  rb_cTilemap = rb_define_class("Tilemap", rb_cObject);
  rb_define_alloc_func(rb_cTilemap, tilemap_alloc);
  rb_define_method(rb_cTilemap, "initialize",
      (VALUE(*)(...))rb_tilemap_initialize, -1);
  rb_define_method(rb_cTilemap, "initialize_copy",
      (VALUE(*)(...))rb_tilemap_initialize_copy, 1);

  rb_define_method(rb_cTilemap, "dup",
      (VALUE(*)(...))rb_tilemap_dup, 1);
  rb_define_method(rb_cTilemap, "clone",
      (VALUE(*)(...))rb_tilemap_clone, 1);

  rb_define_method(rb_cTilemap, "dispose",
      (VALUE(*)(...))rb_tilemap_dispose, 0);
  rb_define_method(rb_cTilemap, "disposed?",
      (VALUE(*)(...))rb_tilemap_disposed, 0);
  rb_define_method(rb_cTilemap, "update",
      (VALUE(*)(...))rb_tilemap_update, 0);

  rb_define_method(rb_cTilemap, "bitmaps",
      (VALUE(*)(...))rb_tilemap_bitmaps, 0);
  rb_define_method(rb_cTilemap, "map_data",
      (VALUE(*)(...))rb_tilemap_map_data, 0);
  rb_define_method(rb_cTilemap, "map_data=",
      (VALUE(*)(...))rb_tilemap_set_map_data, 1);
  rb_define_method(rb_cTilemap, "flash_data",
      (VALUE(*)(...))rb_tilemap_flash_data, 0);
  rb_define_method(rb_cTilemap, "flash_data=",
      (VALUE(*)(...))rb_tilemap_set_flash_data, 1);
  rb_define_method(rb_cTilemap, "flags",
      (VALUE(*)(...))rb_tilemap_flags, 0);
  rb_define_method(rb_cTilemap, "flags=",
      (VALUE(*)(...))rb_tilemap_set_flags, 1);
  rb_define_method(rb_cTilemap, "viewport",
      (VALUE(*)(...))rb_tilemap_viewport, 0);
  rb_define_method(rb_cTilemap, "viewport=",
      (VALUE(*)(...))rb_tilemap_set_viewport, 1);
  rb_define_method(rb_cTilemap, "visible",
      (VALUE(*)(...))rb_tilemap_visible, 0);
  rb_define_method(rb_cTilemap, "visible=",
      (VALUE(*)(...))rb_tilemap_set_visible, 1);
  rb_define_method(rb_cTilemap, "ox",
      (VALUE(*)(...))rb_tilemap_ox, 0);
  rb_define_method(rb_cTilemap, "ox=",
      (VALUE(*)(...))rb_tilemap_set_ox, 1);
  rb_define_method(rb_cTilemap, "oy",
      (VALUE(*)(...))rb_tilemap_oy, 0);
  rb_define_method(rb_cTilemap, "oy=",
      (VALUE(*)(...))rb_tilemap_set_oy, 1);

  rb_cBitmapArray = rb_define_class_under(
      rb_cTilemap, "BitmapArray", rb_cObject);
  rb_define_alloc_func(rb_cBitmapArray, bitmaparray_alloc);

  rb_define_method(rb_cBitmapArray, "[]",
      (VALUE(*)(...))rb_bitmaparray_get, 1);
  rb_define_method(rb_cBitmapArray, "[]=",
      (VALUE(*)(...))rb_bitmaparray_set, 2);
}

Tilemap *convertTilemap(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))tilemap_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Tilemap",
        rb_class2name(rb_obj_class(obj)));
  }
  Tilemap *ret;
  Data_Get_Struct(obj, Tilemap, ret);
  return ret;
}
Tilemap *convertTilemapOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertTilemap(obj);
}
VALUE exportTilemap(Tilemap *ptr) {
  return ptr ? ptr->rb_parent : Qnil;
}

Tilemap *Tilemap::create(Viewport *viewport) {
  VALUE ret = tilemap_alloc(rb_cTilemap);
  Tilemap *ptr = convertTilemap(ret);
  ptr->initialize(viewport);
  return ptr;
}

static void tilemap_mark(Tilemap *ptr) {
  rb_gc_mark(ptr->bitmap_array);
  for(int i = 0; i < 9; ++i) {
    if(ptr->bitmaps[i]) rb_gc_mark(ptr->bitmaps[i]->rb_parent);
  }
  if(ptr->map_data) rb_gc_mark(ptr->map_data->rb_parent);
  if(ptr->flash_data) rb_gc_mark(ptr->flash_data->rb_parent);
  if(ptr->flags) rb_gc_mark(ptr->flags->rb_parent);
  if(ptr->viewport) rb_gc_mark(ptr->viewport->rb_parent);
}

static void tilemap_free(Tilemap *ptr) {
  ptr->dispose();
  xfree(ptr);
}

static VALUE tilemap_alloc(VALUE klass) {
  Tilemap *ptr = ALLOC(Tilemap);
  VALUE ret = Data_Wrap_Struct(klass, tilemap_mark, tilemap_free, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_tilemap_initialize(int argc, VALUE *argv, VALUE self) {
  Tilemap *ptr = convertTilemap(self);
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
static VALUE rb_tilemap_initialize_copy(VALUE self, VALUE orig) {
  rb_raise(rb_eTypeError, "can't dup Tilemap");
  return Qnil;
}
static VALUE rb_tilemap_dup(VALUE self) {
  rb_raise(rb_eTypeError, "can't dup Tilemap");
  return Qnil;
}
static VALUE rb_tilemap_clone(VALUE self) {
  rb_raise(rb_eTypeError, "can't clone Tilemap");
  return Qnil;
}

static VALUE rb_tilemap_dispose(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  ptr->dispose();
  return Qnil;
}
static VALUE rb_tilemap_disposed(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return (ptr->disposed()) ? Qtrue : Qfalse;
}
static VALUE rb_tilemap_update(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  ptr->update();
  return Qnil;
}

static VALUE rb_tilemap_bitmaps(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return ptr->bitmap_array;
}
static VALUE rb_tilemap_map_data(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return exportTable(ptr->map_data);
}
static VALUE rb_tilemap_set_map_data(VALUE self, VALUE map_data) {
  Tilemap *ptr = convertTilemap(self);
  ptr->map_data = convertTableOrNil(map_data);
  return map_data;
}
static VALUE rb_tilemap_flash_data(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return exportTable(ptr->flash_data);
}
static VALUE rb_tilemap_set_flash_data(VALUE self, VALUE flash_data) {
  Tilemap *ptr = convertTilemap(self);
  ptr->flash_data = convertTableOrNil(flash_data);
  return flash_data;
}
static VALUE rb_tilemap_flags(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return exportTable(ptr->flags);
}
static VALUE rb_tilemap_set_flags(VALUE self, VALUE flags) {
  Tilemap *ptr = convertTilemap(self);
  ptr->flags = convertTableOrNil(flags);
  return flags;
}
static VALUE rb_tilemap_viewport(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return exportViewport(ptr->viewport);
}
static VALUE rb_tilemap_set_viewport(VALUE self, VALUE viewport) {
  Tilemap *ptr = convertTilemap(self);
  Graphics::unregister_renderable((Renderable*)ptr, ptr->viewport);
  ptr->viewport = convertViewportOrNil(viewport);
  Graphics::register_renderable((Renderable*)ptr, ptr->viewport);
  return viewport;
}
static VALUE rb_tilemap_visible(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return (ptr->visible) ? Qtrue : Qfalse;
}
static VALUE rb_tilemap_set_visible(VALUE self, VALUE visible) {
  Tilemap *ptr = convertTilemap(self);
  ptr->visible = RTEST(visible);
  return visible;
}
static VALUE rb_tilemap_ox(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return INT2NUM(ptr->ox);
}
static VALUE rb_tilemap_set_ox(VALUE self, VALUE ox) {
  Tilemap *ptr = convertTilemap(self);
  ptr->ox = NUM2INT(ox);
  return ox;
}
static VALUE rb_tilemap_oy(VALUE self) {
  Tilemap *ptr = convertTilemap(self);
  return INT2NUM(ptr->oy);
}
static VALUE rb_tilemap_set_oy(VALUE self, VALUE oy) {
  Tilemap *ptr = convertTilemap(self);
  ptr->oy = NUM2INT(oy);
  return oy;
}

static void bitmaparray_mark(BitmapArray *ptr) {
  if(ptr->tilemap) rb_gc_mark(ptr->tilemap->rb_parent);
}

static VALUE bitmaparray_alloc(VALUE klass) {
  BitmapArray *ptr = ALLOC(BitmapArray);
  VALUE ret = Data_Wrap_Struct(klass, bitmaparray_mark, -1, ptr);
  ptr->tilemap = nullptr;
  return ret;
}

BitmapArray *convertBitmapArray(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))bitmaparray_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into BitmapArray",
        rb_class2name(rb_obj_class(obj)));
  }
  BitmapArray *ret;
  Data_Get_Struct(obj, BitmapArray, ret);
  return ret;
}

static VALUE rb_bitmaparray_get(VALUE self, VALUE index) {
  BitmapArray *ptr = convertBitmapArray(self);
  int index_int = NUM2INT(index);
  if(index_int < 0 || index_int >= 9) return Qnil;
  if(!ptr->tilemap) return Qnil;
  return exportBitmap(ptr->tilemap->bitmaps[index_int]);
}
static VALUE rb_bitmaparray_set(VALUE self, VALUE index, VALUE val) {
  BitmapArray *ptr = convertBitmapArray(self);
  int index_int = NUM2INT(index);
  if(index_int < 0 || index_int >= 9) return Qnil;
  if(!ptr->tilemap) return Qnil;
  ptr->tilemap->bitmaps[index_int] = convertBitmapOrNil(val);
  return Qnil;
}
