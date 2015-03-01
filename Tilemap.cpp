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
  this->sub.renderable_entry.type = RenderableType::TILEMAP_SUB;
  this->sub.renderable_entry.y = 0;
  this->sub.renderable_entry.z = 200;
  this->sub.renderable_entry.renderable_id = current_renderable_id++;
  this->sub.parent = this;
  Graphics::register_renderable((Renderable*)&this->sub, this->viewport);


  for(int i = 0; i < 3; ++i) {
    for(int j = 0; j < 5; ++j) {
      textures[i][j] = nullptr;
    }
  }
  cached_data = nullptr;
}
void Tilemap::dispose() {
  if(!this->is_disposed) {
    Graphics::unregister_renderable((Renderable*)this, this->viewport);
    Graphics::unregister_renderable((Renderable*)&this->sub, this->viewport);
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

static int autotilemap[4][96] = {
  { 18,  2, 18,  2, 18,  2, 18,  2, 18,  2, 18,  2, 18,  2, 18,  2,
    16, 16, 16, 16, 10, 10, 10, 10, 18, 18,  2,  2, 18,  2, 18,  2,
    16, 10,  8,  8, 10, 10, 18,  2, 16, 16,  8,  8, 16, 10,  8,  0,
    10,  8,  2,  0, 10,  8,  2,  0, 10,  8,  2,  0, 10,  8,  2,  0},
  { 17, 17,  3,  3, 17, 17,  3,  3, 17, 17,  3,  3, 17, 17,  3,  3,
    17,  3, 17,  3,  9,  9,  9,  9, 19, 19, 19, 19, 17, 17,  3,  3,
    19,  9,  9,  9, 11, 11, 19, 19, 17,  3, 11,  9, 19, 11, 11,  1,
     9,  9,  1,  1, 11, 11,  3,  3,  9,  9,  1,  1, 11, 11,  3,  3},
  { 14, 14, 14, 14, 14, 14, 14, 14,  6,  6,  6,  6,  6,  6,  6,  6,
    12, 12, 12, 12, 14, 14,  6,  6, 14,  6, 14,  6, 22, 22, 22, 22,
    12, 22, 12, 12, 14,  6, 22, 22, 20, 20, 12, 20, 20, 22, 20,  4,
     6,  4,  6,  4,  6,  4,  6,  4, 14, 12, 14, 12, 14, 12, 14, 12},
  { 13, 13, 13, 13,  7,  7,  7,  7, 13, 13, 13, 13,  7,  7,  7,  7,
    13, 13,  7,  7, 13,  7, 13,  7, 15, 15, 15, 15, 21, 21, 21, 21,
    15, 21, 13,  7, 15, 15, 23, 23, 21, 21, 15, 21, 23, 23, 23,  5,
     5,  5,  5,  5,  7,  7,  7,  7, 13, 13, 13, 13, 15, 15, 15, 15}};

void Tilemap::render(
    SDL_Renderer *renderer,
    int rox, int roy, int rwidth, int rheight, bool is_sub) {
  if(!this->visible || !this->map_data) return;
  int zsize = this->map_data->zsize;
  int ysize = this->map_data->ysize;
  int xsize = this->map_data->xsize;
  if(ysize == 0 || xsize == 0) return;
  bool updated = false;
  if(!cached_data) {
    updated = true;
  } else if(
      cached_xsize != xsize ||
      cached_ysize != ysize ||
      cached_zsize != zsize) {
    updated = true;
  } else {
    int size = xsize*ysize*zsize;
    for(int i = 0; i < size; ++i) {
      if(map_data->data[i] != cached_data[i]) {
        updated = true;
      }
    }
  }
  if(cached_data && updated) {
    for(int i = 0; i < 3; ++i) {
      for(int j = 0; j < 5; ++j) {
        SDL_DestroyTexture(textures[i][j]);
        textures[i][j] = nullptr;
      }
    }
    delete[] cached_data;
    cached_data = nullptr;
  }
  if(updated) {
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
    cached_xsize = xsize;
    cached_ysize = ysize;
    cached_zsize = zsize;
    cached_data = new short[xsize*ysize*zsize];
    int size = xsize*ysize*zsize;
    for(int i = 0; i < size; ++i) {
      cached_data[i] = map_data->data[i];
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
      int zi = j >= 3 ? 2 : j == 2 ? 3 : j;
      for(int yi = 0; yi < ysize; ++yi) {
        for(int xi = 0; xi < xsize; ++xi) {
          int tileid = this->map_data->get(xi, yi, zi);
          if(j == 2) {
            SDL_Rect dst_rect;
            for(int off = 0; off < 4; ++off) {
              dst_rect = {xi*32+16*(off&1), yi*32+16*((off>>1)&1), 16, 16};
              SDL_FillRect(surface, &dst_rect,
                  SDL_MapRGBA(surface->format, 0, 0, 0,
                    (tileid&(1<<off)) ? 128 : 0));
            }
            continue;
          }
          int flag = this->flags ? this->flags->get(tileid) : 0;
          if((flag&0x0010) && j==3) continue;
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
          } else if(0x0600 <= tileid && tileid < 0x0680) {
            int bitmapid = 4;
            int ti = tileid-0x0600;
            SDL_Rect src_rect = {0, 0, 32, 32};
            src_rect.x = (ti&7)*32;
            src_rect.y = ((ti>>3)&15)*32;
            SDL_Rect dst_rect = {xi*32, yi*32, 32, 32};
            if(!this->bitmaps[bitmapid]) continue;
            SDL_SetSurfaceBlendMode(
                this->bitmaps[bitmapid]->surface, SDL_BLENDMODE_NONE);
            SDL_BlitSurface(
                this->bitmaps[bitmapid]->surface, &src_rect,
                surface, &dst_rect);
          } else if(0x0800 <= tileid && tileid < 0x2000) {
            int autoidx = (tileid-0x0800)%0x0030;
            int ti = (tileid-0x0800)/0x0030;
            int bitmapid;
            int atx, aty;
            if(ti < 16) {
              bitmapid = 0;
              int tii = ti;
              if(tii == 1 || tii == 2) tii ^= 3;
              if((tii&1) == 0) {
                atx = ((tii&4)<<1) + i*2;
              } else {
                atx = ((tii&4)<<1) + 3*2;
              }
              aty = ((tii>>1)&1)*3 + ((tii>>3)&1)*6;
            } else if(ti < 48) {
              bitmapid = 1;
              int tii = ti-16;
              atx = (tii&7)<<1;
              aty = ((tii>>3)&3)*3;
            } else if(ti < 80) {
              bitmapid = 2;
              int tii = ti-48;
              autoidx += 48;
              atx = (tii&7)<<1;
              aty = ((tii>>3)&3)<<1;
            } else {
              bitmapid = 3;
              int tii = ti-80;
              atx = (tii&7)<<1;
              aty = ((tii>>4)&3)*5;
              if(tii&8) {
                autoidx += 48;
                aty += 3;
              }
            }
            for(int off = 0; off < 4; ++off) {
              int k = autotilemap[off][autoidx];
              SDL_Rect src_rect = {0, 0, 16, 16};
              src_rect.x = atx*32+(k&3)*16;
              src_rect.y = aty*32+(k>>2)*16;
              SDL_Rect dst_rect = {0, 0, 16, 16};
              dst_rect.x = xi*32+16*(off&1);
              dst_rect.y = yi*32+16*((off>>1)&1);
              if(!this->bitmaps[bitmapid]) continue;
              SDL_SetSurfaceBlendMode(
                  this->bitmaps[bitmapid]->surface, SDL_BLENDMODE_NONE);
              SDL_BlitSurface(
                  this->bitmaps[bitmapid]->surface, &src_rect,
                  surface, &dst_rect);
            }
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
    if(is_sub && j < 4) continue;
    if(!is_sub && j >= 4) continue;
    SDL_Rect dst_rect;
    dst_rect.x = -ox-rox;
    dst_rect.y = -oy-roy;
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
  for(int i = 0; i < 9; ++i) {
    ptr->bitmaps[i] = nullptr;
  }
  ptr->map_data = nullptr;
  ptr->flash_data = nullptr;
  ptr->flags = nullptr;
  ptr->viewport = nullptr;
  for(int i = 0; i < 3; ++i) {
    for(int j = 0; j < 5; ++j) {
      ptr->textures[i][j] = nullptr;
    }
  }
  ptr->cached_data = nullptr;
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
  Graphics::unregister_renderable((Renderable*)&ptr->sub, ptr->viewport);
  ptr->viewport = convertViewportOrNil(viewport);
  Graphics::register_renderable((Renderable*)ptr, ptr->viewport);
  Graphics::register_renderable((Renderable*)&ptr->sub, ptr->viewport);
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
