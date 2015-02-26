#ifndef TILEMAP_H_INCLUDED
#define TILEMAP_H_INCLUDED

#include <ruby.h>
#include <SDL.h>

#include "Bitmap.h"
#include "Table.h"
#include "Viewport.h"
#include "renderable.h"

struct Tilemap {
  struct Renderable renderable_entry;
  VALUE rb_parent;

  VALUE bitmap_array;
  Bitmap *bitmaps[9];
  Table *map_data, *flash_data, *flags;
  Viewport *viewport;
  bool visible;
  int ox, oy;

  bool is_disposed;

  void initialize(Viewport *viewport = nullptr);
  void dispose();
  bool disposed();
  void update();

  static Tilemap *create(Viewport *viewport = nullptr);

  void render(SDL_Renderer *renderer);
};

extern VALUE rb_cTilemap;
extern VALUE rb_cBitmapArray;
extern void InitTilemap();

Tilemap *convertTilemap(VALUE obj);
Tilemap *convertTilemapOrNil(VALUE obj);
VALUE exportTilemap(Tilemap *ptr);

struct BitmapArray {
  Tilemap *tilemap;
};

BitmapArray *convertBitmapArray(VALUE obj);

#endif // TILEMAP_H_INCLUDED
