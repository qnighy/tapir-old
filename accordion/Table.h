#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include <stdint.h>

#include <ruby.h>

extern VALUE rb_cTable;
extern void InitTable(void);

VALUE rb_table_new(int32_t dim, int32_t xsize, int32_t ysize, int32_t zsize);

void rb_table_resize(
    VALUE self, int32_t new_dim, int32_t new_xsize,
    int32_t new_ysize, int32_t new_zsize);

int32_t rb_table_dim(VALUE self);

int32_t rb_table_xsize(VALUE self);

int32_t rb_table_ysize(VALUE self);

int32_t rb_table_zsize(VALUE self);

int16_t rb_table_aref(VALUE self, int32_t x, int32_t y, int32_t z);

void rb_table_aset(VALUE self, int32_t x, int32_t y, int32_t z, int16_t val);

short *rb_table_data(VALUE self);

struct Table {
  int dim, xsize, ysize, zsize, size;
  short *data;
  void resize_internal(
      int new_dim, int new_xsize, int new_ysize, int new_zsize);
  void initialize(int xsize, int ysize, int zsize);
  void initialize(int xsize, int ysize);
  void initialize(int xsize);
  void resize(int xsize, int ysize, int zsize);
  void resize(int xsize, int ysize);
  void resize(int xsize);
  short get_internal(int dim, int x, int y, int z);
  short get(int x, int y, int z);
  short get(int x, int y);
  short get(int x);
  void set_internal(int dim, int x, int y, int z, short val);
  void set(int x, int y, int z, short val);
  void set(int x, int y, short val);
  void set(int x, short val);

  static Table *create(int xsize, int ysize, int zsize);
  static Table *create(int xsize, int ysize);
  static Table *create(int xsize);
};

Table *convertTable(VALUE obj);
Table *convertTableOrNil(VALUE obj);

#endif // TABLE_H_INCLUDED
