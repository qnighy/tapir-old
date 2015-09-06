#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include <stdint.h>

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef VALUE TableVALUE;

extern VALUE rb_cTable;
extern void InitTable(void);

TableVALUE rb_table_new(
    int32_t dim, int32_t xsize, int32_t ysize, int32_t zsize);

void rb_table_resize(
    TableVALUE self, int32_t new_dim, int32_t new_xsize,
    int32_t new_ysize, int32_t new_zsize);

int32_t rb_table_dim(TableVALUE self);

int32_t rb_table_xsize(TableVALUE self);

int32_t rb_table_ysize(TableVALUE self);

int32_t rb_table_zsize(TableVALUE self);

int16_t rb_table_aref(TableVALUE self, int32_t x, int32_t y, int32_t z);

void rb_table_aset(
    TableVALUE self, int32_t x, int32_t y, int32_t z, int16_t val);

int16_t *rb_table_data(TableVALUE self);

#ifdef __cplusplus
}
#endif

#endif // TABLE_H_INCLUDED
