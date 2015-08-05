#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include <ruby.h>

struct Table {
  VALUE rb_parent;
  int dim, xsize, ysize, zsize;
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

extern VALUE rb_cTable;
extern void InitTable();

Table *convertTable(VALUE obj);
Table *convertTableOrNil(VALUE obj);
VALUE exportTable(Table *ptr);

#endif // TABLE_H_INCLUDED
