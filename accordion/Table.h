#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include <stdint.h>

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

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

int16_t *rb_table_data(VALUE self);

#ifdef __cplusplus
}
#endif

#endif // TABLE_H_INCLUDED
