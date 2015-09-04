#include <algorithm>

#include "Table.h"
#include "misc.h"

struct Table {
  int dim, xsize, ysize, zsize, size;
  short *data;
};

Table *convertTable(VALUE obj);
static void table_mark(Table *);
static void table_free(Table *);
static VALUE table_alloc(VALUE klass);

VALUE rb_table_new(int32_t dim, int32_t xsize, int32_t ysize, int32_t zsize) {
  VALUE ret = table_alloc(rb_cTable);
  struct Table *ptr = convertTable(ret);
  ptr->dim = dim;
  ptr->xsize = xsize;
  ptr->ysize = ysize;
  ptr->zsize = zsize;
  ptr->size = xsize * ysize * zsize;
  ptr->data = new short[ptr->size];
  std::fill_n(ptr->data, ptr->size, 0);
  return ret;
}

void rb_table_resize(
    VALUE self, int32_t new_dim, int32_t new_xsize,
    int32_t new_ysize, int32_t new_zsize) {
  struct Table *ptr = convertTable(self);
  int32_t new_size = new_xsize*new_ysize*new_zsize;
  short *new_data = new short[new_size];
  std::fill_n(new_data, new_size, 0);
  if(ptr->data) {
    int xsize_min = std::min(ptr->xsize, new_xsize);
    int ysize_min = std::min(ptr->ysize, new_ysize);
    int zsize_min = std::min(ptr->zsize, new_zsize);
    for(int z = 0; z < zsize_min; ++z) {
      for(int y = 0; y < ysize_min; ++y) {
        for(int x = 0; x < xsize_min; ++x) {
          new_data[(z*new_ysize+y)*new_xsize+x] =
            ptr->data[(z*ptr->ysize+y)*ptr->xsize+x];
        }
      }
    }
    delete[] ptr->data;
  }
  ptr->dim = new_dim;
  ptr->xsize = new_xsize;
  ptr->ysize = new_ysize;
  ptr->zsize = new_zsize;
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
    ptr->data[((z * ptr->ysize) + y) * ptr->xsize + x] = val;
  }
}

short *rb_table_data(VALUE self) {
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
  rb_define_method(rb_cTable, "initialize",
      (VALUE(*)(...))rb_table_m_initialize, -1);
  rb_define_method(rb_cTable, "initialize_copy",
      (VALUE(*)(...))rb_table_m_initialize_copy, 1);
  rb_define_method(rb_cTable, "resize",
      (VALUE(*)(...))rb_table_m_resize, -1);
  rb_define_method(rb_cTable, "[]",
      (VALUE(*)(...))rb_table_m_get, -1);
  rb_define_method(rb_cTable, "[]=",
      (VALUE(*)(...))rb_table_m_set, -1);

  rb_define_singleton_method(rb_cTable, "_load",
      (VALUE(*)(...))rb_table_m_old_load, 1);
  rb_define_method(rb_cTable, "_dump",
      (VALUE(*)(...))rb_table_m_old_dump, 1);
}

Table *convertTable(VALUE obj) {
  Check_Type(obj, T_DATA);
  if(RDATA(obj)->dmark != (void(*)(void*))table_mark) {
    rb_raise(rb_eTypeError,
        "can't convert %s into Table",
        rb_class2name(rb_obj_class(obj)));
  }
  Table *ret;
  Data_Get_Struct(obj, Table, ret);
  return ret;
}

static void table_mark(Table *) {}
static void table_free(Table *ptr) {
  if(ptr->data) delete[] ptr->data;
  xfree(ptr);
}

static VALUE table_alloc(VALUE klass) {
  Table *ptr = ALLOC(Table);
  ptr->data = nullptr;
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
  Table *ptr = convertTable(self);
  Table *orig_ptr = convertTable(orig);
  ptr->dim = orig_ptr->dim;
  ptr->xsize = orig_ptr->xsize;
  ptr->ysize = orig_ptr->ysize;
  ptr->zsize = orig_ptr->zsize;
  ptr->size = orig_ptr->size;
  ptr->data = new short[ptr->size];
  std::copy_n(orig_ptr->data, ptr->size, ptr->data);
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
  Table *ptr = convertTable(self);
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
  Table *ptr = convertTable(self);
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

static VALUE rb_table_m_old_load(VALUE, VALUE str) {
  VALUE ret = table_alloc(rb_cTable);
  Table *ptr = convertTable(ret);
  ptr->data = nullptr;
  str = StringValue(str);
  char *s = StringValuePtr(str);
  if(!s) {
    fprintf(stderr, "warning: broken marshal data of Table\n");
    return ret;
  }
  int len = RSTRING_LEN(str);
  if(len < 20) {
    fprintf(stderr, "warning: broken marshal data of Table\n");
    return ret;
  }
  // TODO make it more safe
  ptr->dim = readInt32(s);
  ptr->xsize = readInt32(s+4);
  ptr->ysize = readInt32(s+8);
  ptr->zsize = readInt32(s+12);
  ptr->size = readInt32(s+16);
  if(len < 20+ptr->size*2) {
    fprintf(stderr, "warning: broken marshal data of Table\n");
    return ret;
  }
  ptr->data = new short[ptr->size];
  for(int i = 0; i < ptr->size; ++i) {
    ptr->data[i] = readInt16(s+20+i*2);
  }
  return ret;
}

static VALUE rb_table_m_old_dump(VALUE self, VALUE) {
  Table *ptr = convertTable(self);
  char *s = new char[20+2*ptr->size];
  writeInt32(s, ptr->dim);
  writeInt32(s+4, ptr->xsize);
  writeInt32(s+8, ptr->ysize);
  writeInt32(s+12, ptr->zsize);
  writeInt32(s+16, ptr->size);
  for(int i = 0; i < ptr->size; ++i) {
    writeInt16(s+20+i*2, ptr->data[i]);
  }
  VALUE ret = rb_str_new(s, 20+2*ptr->size);
  delete[] s;
  return ret;
}
