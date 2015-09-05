#include "Table.h"
#include "misc.h"

struct Table {
  int32_t dim, xsize, ysize, zsize, size;
  int16_t *data;
};

struct Table *convertTable(VALUE obj);
static void rb_table_modify(VALUE obj);
static void table_mark(struct Table *);
static void table_free(struct Table *);
static VALUE table_alloc(VALUE klass);

#define SQRT_UINT32_MAX ((uint32_t)1<<16)

static int32_t multiply_size(
    uint32_t xsize, uint32_t ysize, uint32_t zsize) {
#ifdef CORRECT_RGSS_BEHAVIOR
  if((xsize >= SQRT_UINT32_MAX || ysize >= SQRT_UINT32_MAX) &&
      xsize > 0 && UINT32_MAX / xsize < ysize) {
    goto fail;
  }
  uint32_t xysize = xsize * ysize;
  if((xysize >= SQRT_UINT32_MAX || zsize >= SQRT_UINT32_MAX) &&
      xysize > 0 && UINT32_MAX / xysize < zsize) {
    goto fail;
  }
  uint32_t size = xysize * zsize;
  if(size > (INT32_MAX - 20) / 2) {
    goto fail;
  }
  return xsize * ysize * zsize;
fail:
  rb_raise(rb_eArgError, "Multiplied table size is too large.");
#else
  return xsize * ysize * zsize;
#endif
}

VALUE rb_table_new(int32_t dim, int32_t xsize, int32_t ysize, int32_t zsize) {
  if(xsize < 0) xsize = 0;
  if(ysize < 0) ysize = 0;
  if(zsize < 0) zsize = 0;
  int32_t size = multiply_size(xsize, ysize, zsize);
  VALUE ret = table_alloc(rb_cTable);
  struct Table *ptr = convertTable(ret);
  ptr->dim = dim;
  ptr->xsize = xsize;
  ptr->ysize = ysize;
  ptr->zsize = zsize;
  ptr->size = size;
  ptr->data = ALLOC_N(int16_t, size);
  for(int32_t i = 0; i < size; ++i) {
    ptr->data[i] = 0;
  }
  return ret;
}

void rb_table_resize(
    VALUE self, int32_t new_dim, int32_t new_xsize,
    int32_t new_ysize, int32_t new_zsize) {
  struct Table *ptr = convertTable(self);
  if(new_xsize < 0) new_xsize = 0;
  if(new_ysize < 0) new_ysize = 0;
  if(new_zsize < 0) new_zsize = 0;
  rb_table_modify(self);
  int32_t new_size = multiply_size(new_xsize, new_ysize, new_zsize);
  int16_t *new_data = ALLOC_N(int16_t, new_size);
  for(int32_t i = 0; i < new_size; ++i) {
    new_data[i] = 0;
  }
  if(ptr->data) {
    int32_t xsize_min = ptr->xsize < new_xsize ? ptr->xsize : new_xsize;
    int32_t ysize_min = ptr->ysize < new_ysize ? ptr->ysize : new_ysize;
    int32_t zsize_min = ptr->zsize < new_zsize ? ptr->zsize : new_zsize;
    for(int32_t z = 0; z < zsize_min; ++z) {
      for(int32_t y = 0; y < ysize_min; ++y) {
        for(int32_t x = 0; x < xsize_min; ++x) {
          new_data[(z*new_ysize+y)*new_xsize+x] =
            ptr->data[(z*ptr->ysize+y)*ptr->xsize+x];
        }
      }
    }
    xfree(ptr->data);
  }
  ptr->dim = new_dim;
  ptr->xsize = new_xsize;
  ptr->ysize = new_ysize;
  ptr->zsize = new_zsize;
  ptr->size = new_size;
  ptr->data = new_data;
}

int32_t rb_table_dim(VALUE self) {
  struct Table *ptr = convertTable(self);
  return ptr->dim;
}

int32_t rb_table_xsize(VALUE self) {
  struct Table *ptr = convertTable(self);
  return ptr->xsize;
}

int32_t rb_table_ysize(VALUE self) {
  struct Table *ptr = convertTable(self);
  return ptr->ysize;
}

int32_t rb_table_zsize(VALUE self) {
  struct Table *ptr = convertTable(self);
  return ptr->zsize;
}

int16_t rb_table_aref(VALUE self, int32_t x, int32_t y, int32_t z) {
  struct Table *ptr = convertTable(self);
  if(0 <= x && x < ptr->xsize && 0 <= y && y < ptr->ysize &&
      0 <= z && z < ptr->zsize) {
    return ptr->data[((z * ptr->ysize) + y) * ptr->xsize + x];
  } else {
    return Qnil;
  }
}

void rb_table_aset(VALUE self, int32_t x, int32_t y, int32_t z, int16_t val) {
  struct Table *ptr = convertTable(self);
  if(0 <= x && x < ptr->xsize && 0 <= y && y < ptr->ysize &&
      0 <= z && z < ptr->zsize) {
    rb_table_modify(self);
    ptr->data[((z * ptr->ysize) + y) * ptr->xsize + x] = val;
  }
}

int16_t *rb_table_data(VALUE self) {
  struct Table *ptr = convertTable(self);
  return ptr->data;
}

static VALUE rb_table_m_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_m_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_table_m_resize(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_m_get(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_m_set(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_m_old_load(VALUE self, VALUE s);
static VALUE rb_table_m_old_dump(VALUE self, VALUE lim);

VALUE rb_cTable;
void InitTable() {
  rb_cTable = rb_define_class("Table", rb_cObject);
  rb_define_alloc_func(rb_cTable, table_alloc);
  rb_define_private_method(rb_cTable, "initialize", rb_table_m_initialize, -1);
  rb_define_private_method(rb_cTable, "initialize_copy",
      rb_table_m_initialize_copy, 1);
  rb_define_method(rb_cTable, "resize", rb_table_m_resize, -1);
  rb_define_method(rb_cTable, "[]", rb_table_m_get, -1);
  rb_define_method(rb_cTable, "[]=", rb_table_m_set, -1);

  rb_define_singleton_method(rb_cTable, "_load", rb_table_m_old_load, 1);
  rb_define_method(rb_cTable, "_dump", rb_table_m_old_dump, 1);
}

struct Table *convertTable(VALUE obj) {
  Check_Type(obj, T_DATA);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(RDATA(obj)->dmark != (void(*)(void*))table_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Table",
        rb_class2name(rb_obj_class(obj)));
  }
#endif
  struct Table *ret;
  Data_Get_Struct(obj, struct Table, ret);
  return ret;
}

static void rb_table_modify(VALUE obj) {
#ifdef CORRECT_RGSS_BEHAVIOR
  if(OBJ_FROZEN(obj)) rb_error_frozen("Table");
  if(!OBJ_UNTRUSTED(obj) && rb_safe_level() >= 4) {
    rb_raise(rb_eSecurityError, "Insecure: can't modify Table");
  }
#endif
}

static void table_mark(struct Table *ptr) {}
static void table_free(struct Table *ptr) {
  if(ptr->data) xfree(ptr->data);
  xfree(ptr);
}

static VALUE table_alloc(VALUE klass) {
  struct Table *ptr = ALLOC(struct Table);
  ptr->dim = 0;
  ptr->xsize = 0;
  ptr->ysize = 0;
  ptr->zsize = 0;
  ptr->size = 0;
  ptr->data = NULL;
  VALUE ret = Data_Wrap_Struct(klass, table_mark, table_free, ptr);
  return ret;
}

static VALUE rb_table_m_initialize(int argc, VALUE *argv, VALUE self) {
  if(1 <= argc && argc <= 3) {
    rb_table_resize(
        self, argc,
        0 < argc ? NUM2INT(argv[0]) : 1,
        1 < argc ? NUM2INT(argv[1]) : 1,
        2 < argc ? NUM2INT(argv[2]) : 1);
  } else {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for 1..3)", argc);
  }
  return Qnil;
}
static VALUE rb_table_m_initialize_copy(VALUE self, VALUE orig) {
  struct Table *ptr = convertTable(self);
  struct Table *orig_ptr = convertTable(orig);
  rb_table_modify(self);
  ptr->dim = orig_ptr->dim;
  ptr->xsize = orig_ptr->xsize;
  ptr->ysize = orig_ptr->ysize;
  ptr->zsize = orig_ptr->zsize;
  ptr->size = orig_ptr->size;
  ptr->data = ALLOC_N(int16_t, ptr->size);
  for(int32_t i = 0; i < ptr->size; ++i) {
    ptr->data[i] = orig_ptr->data[i];
  }
  return Qnil;
}

static VALUE rb_table_m_resize(int argc, VALUE *argv, VALUE self) {
  if(1 <= argc && argc <= 3) {
    rb_table_resize(
        self, argc,
        0 < argc ? NUM2INT(argv[0]) : 1,
        1 < argc ? NUM2INT(argv[1]) : 1,
        2 < argc ? NUM2INT(argv[2]) : 1);
  } else {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for 1..3)", argc);
  }
  return Qnil;
}
static VALUE rb_table_m_get(int argc, VALUE *argv, VALUE self) {
  struct Table *ptr = convertTable(self);
  if(argc == ptr->dim) {
    return INT2NUM((int)rb_table_aref(
          self,
          0 < argc ? NUM2INT(argv[0]) : 0,
          1 < argc ? NUM2INT(argv[1]) : 0,
          2 < argc ? NUM2INT(argv[2]) : 0));
  } else {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for %d)", argc, ptr->dim);
  }
  return Qnil;
}
static VALUE rb_table_m_set(int argc, VALUE *argv, VALUE self) {
  struct Table *ptr = convertTable(self);
  if(argc == ptr->dim+1) {
    rb_table_aset(
        self,
        0 < argc-1 ? NUM2INT(argv[0]) : 0,
        1 < argc-1 ? NUM2INT(argv[1]) : 0,
        2 < argc-1 ? NUM2INT(argv[2]) : 0,
        NUM2INT(argv[argc-1]));
  } else {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for %d)", argc, ptr->dim+1);
  }
  return Qnil;
}

static VALUE rb_table_m_old_load(VALUE klass, VALUE str) {
  VALUE ret = table_alloc(rb_cTable);
  struct Table *ptr = convertTable(ret);
  StringValue(str);
#ifdef CORRECT_RGSS_BEHAVIOR
  Check_Type(str, T_STRING);
#endif
  const char *s = RSTRING_PTR(str);
#ifdef CORRECT_RGSS_BEHAVIOR
  long s_len = RSTRING_LEN(str);
  if(s_len < sizeof(int32_t)*5) {
    rb_raise(rb_eArgError, "Corrupted marshal data for Table.");
  }
#else
  if(!s) s = "";
#endif
  ptr->dim = readInt32(s+sizeof(int32_t)*0);
  ptr->xsize = readInt32(s+sizeof(int32_t)*1);
  ptr->ysize = readInt32(s+sizeof(int32_t)*2);
  ptr->zsize = readInt32(s+sizeof(int32_t)*3);
  ptr->size = readInt32(s+sizeof(int32_t)*4);
#ifdef CORRECT_RGSS_BEHAVIOR
  if(ptr->dim < 1 || ptr->dim > 3 ||
      ptr->xsize < 0 || ptr->ysize < 0 || ptr->zsize < 0) {
    rb_raise(rb_eArgError, "Corrupted marshal data for Table.");
  }
  if(ptr->size != multiply_size(ptr->xsize, ptr->ysize, ptr->zsize)) {
    rb_raise(rb_eArgError, "Corrupted marshal data for Table.");
  }
  if(len - sizeof(int32_t)*5 != sizeof(int16_t)*ptr->size) {
    rb_raise(rb_eArgError, "Corrupted marshal data for Table.");
  }
#endif
  rb_table_modify(ret);
  if(ptr->data) xfree(ptr->data);
  ptr->data = ALLOC_N(int16_t, ptr->size);
  for(int32_t i = 0; i < ptr->size; ++i) {
    ptr->data[i] = readInt16(s+sizeof(int32_t)*5+sizeof(int16_t)*i);
  }
  return ret;
}

static VALUE rb_table_m_old_dump(VALUE self, VALUE limit) {
  struct Table *ptr = convertTable(self);
  size_t dumpsize = sizeof(int32_t)*5+sizeof(int16_t)*(ptr->size);
  char *s = (char *)xmalloc(dumpsize);
  writeInt32(s, ptr->dim);
  writeInt32(s+4, ptr->xsize);
  writeInt32(s+8, ptr->ysize);
  writeInt32(s+12, ptr->zsize);
  writeInt32(s+16, ptr->size);
  for(int32_t i = 0; i < ptr->size; ++i) {
    writeInt16(s+20+i*2, ptr->data[i]);
  }
  VALUE ret = rb_str_new(s, dumpsize);
  xfree(s);
  return ret;
}
