#include <algorithm>

#include "Table.h"
#include "misc.h"

void Table::resize_internal(int new_dim, int new_xsize, int new_ysize, int new_zsize) {
  short *new_data = new short[new_xsize*new_ysize*new_zsize];
  std::fill_n(new_data, new_xsize*new_ysize*new_zsize, 0);
  if(data) {
    int xsize_min = std::min(xsize, new_xsize);
    int ysize_min = std::min(ysize, new_ysize);
    int zsize_min = std::min(zsize, new_zsize);
    for(int z = 0; z < zsize_min; ++z) {
      for(int y = 0; y < ysize_min; ++y) {
        for(int x = 0; x < xsize_min; ++x) {
          new_data[(z*new_ysize+y)*new_xsize+x] =
            data[(z*ysize+y)*xsize+x];
        }
      }
    }
    delete[] data;
  }
  dim = new_dim;
  xsize = new_xsize;
  ysize = new_ysize;
  zsize = new_zsize;
  data = new_data;
}
void Table::initialize(int xsize, int ysize, int zsize) {
  data = nullptr;
  resize_internal(3, xsize, ysize, zsize);
}
void Table::initialize(int xsize, int ysize) {
  data = nullptr;
  resize_internal(2, xsize, ysize, 1);
}
void Table::initialize(int xsize) {
  data = nullptr;
  resize_internal(1, xsize, 1, 1);
}
void Table::resize(int xsize, int ysize, int zsize) {
  resize_internal(3, xsize, ysize, zsize);
}
void Table::resize(int xsize, int ysize) {
  resize_internal(2, xsize, ysize, 1);
}
void Table::resize(int xsize) {
  resize_internal(1, xsize, 1, 1);
}
short Table::get_internal(int dim, int x, int y, int z) {
  // if(dim != this->dim) ...
  return data[(z*ysize+y)*xsize+x];
}
short Table::get(int x, int y, int z) {
  return get_internal(3, x, y, z);
}
short Table::get(int x, int y) {
  return get_internal(2, x, y, 0);
}
short Table::get(int x) {
  return get_internal(1, x, 0, 0);
}
void Table::set_internal(int dim, int x, int y, int z, short val) {
  // if(dim != this->dim) ...
  data[(z*ysize+y)*xsize+x] = val;
}
void Table::set(int x, int y, int z, short val) {
  set_internal(3, x, y, z, val);
}
void Table::set(int x, int y, short val) {
  set_internal(2, x, y, 0, val);
}
void Table::set(int x, short val) {
  set_internal(1, x, 0, 0, val);
}

static void table_mark(Table *);
static void table_free(Table *);
static VALUE table_alloc(VALUE klass);

static VALUE rb_table_initialize(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_initialize_copy(VALUE self, VALUE orig);

static VALUE rb_table_resize(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_get(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_set(int argc, VALUE *argv, VALUE self);
static VALUE rb_table_old_load(VALUE self, VALUE s);
static VALUE rb_table_old_dump(VALUE self, VALUE lim);

VALUE rb_cTable;
void InitTable() {
  rb_cTable = rb_define_class("Table", rb_cObject);
  rb_define_alloc_func(rb_cTable, table_alloc);
  rb_define_method(rb_cTable, "initialize",
      (VALUE(*)(...))rb_table_initialize, -1);
  rb_define_method(rb_cTable, "initialize_copy",
      (VALUE(*)(...))rb_table_initialize_copy, 1);
  rb_define_method(rb_cTable, "resize",
      (VALUE(*)(...))rb_table_resize, -1);
  rb_define_method(rb_cTable, "[]",
      (VALUE(*)(...))rb_table_get, -1);
  rb_define_method(rb_cTable, "[]=",
      (VALUE(*)(...))rb_table_set, -1);

  rb_define_singleton_method(rb_cTable, "_load",
      (VALUE(*)(...))rb_table_old_load, 1);
  rb_define_method(rb_cTable, "_dump",
      (VALUE(*)(...))rb_table_old_dump, 1);
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
Table *convertTableOrNil(VALUE obj) {
  if(NIL_P(obj)) return NULL;
  return convertTable(obj);
}
VALUE exportTable(Table *ptr) {
  return ptr ? ptr->rb_parent : Qnil;
}

Table *Table::create(int xsize, int ysize, int zsize) {
  VALUE ret = table_alloc(rb_cTable);
  Table *ptr = convertTable(ret);
  ptr->initialize(xsize, ysize, zsize);
  return ptr;
}
Table *Table::create(int xsize, int ysize) {
  VALUE ret = table_alloc(rb_cTable);
  Table *ptr = convertTable(ret);
  ptr->initialize(xsize, ysize);
  return ptr;
}
Table *Table::create(int xsize) {
  VALUE ret = table_alloc(rb_cTable);
  Table *ptr = convertTable(ret);
  ptr->initialize(xsize);
  return ptr;
}

static void table_mark(Table *) {}
static void table_free(Table *ptr) {
  if(ptr->data) delete[] ptr->data;
  xfree(ptr);
}

static VALUE table_alloc(VALUE klass) {
  Table *ptr = ALLOC(Table);
  VALUE ret = Data_Wrap_Struct(klass, table_mark, table_free, ptr);
  ptr->rb_parent = ret;
  return ret;
}

static VALUE rb_table_initialize(int argc, VALUE *argv, VALUE self) {
  Table *ptr = convertTable(self);
  ptr->data = nullptr;
  if(1 <= argc && argc <= 3) {
    ptr->resize_internal(
        argc,
        0 < argc ? NUM2INT(argv[0]) : 1,
        1 < argc ? NUM2INT(argv[1]) : 1,
        2 < argc ? NUM2INT(argv[2]) : 1);
  } else {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for 1..3)", argc);
  }
  return Qnil;
}
static VALUE rb_table_initialize_copy(VALUE self, VALUE orig) {
  Table *ptr = convertTable(self);
  Table *orig_ptr = convertTable(orig);
  ptr->dim = orig_ptr->dim;
  ptr->xsize = orig_ptr->xsize;
  ptr->ysize = orig_ptr->ysize;
  ptr->zsize = orig_ptr->zsize;
  ptr->data = new short[ptr->xsize * ptr->ysize * ptr->zsize];
  std::copy_n(orig_ptr->data, ptr->xsize * ptr->ysize * ptr->zsize,
      ptr->data);
  return Qnil;
}

static VALUE rb_table_resize(int argc, VALUE *argv, VALUE self) {
  Table *ptr = convertTable(self);
  if(1 <= argc && argc <= 3) {
    ptr->resize_internal(
        argc,
        0 < argc ? NUM2INT(argv[0]) : 1,
        1 < argc ? NUM2INT(argv[1]) : 1,
        2 < argc ? NUM2INT(argv[2]) : 1);
  } else {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for 1..3)", argc);
  }
  return Qnil;
}
static VALUE rb_table_get(int argc, VALUE *argv, VALUE self) {
  Table *ptr = convertTable(self);
  if(argc == ptr->dim) {
    short ret = ptr->get_internal(
        argc,
        0 < argc ? NUM2INT(argv[0]) : 0,
        1 < argc ? NUM2INT(argv[1]) : 0,
        2 < argc ? NUM2INT(argv[2]) : 0);
    return INT2NUM((int)ret);
  } else {
    rb_raise(rb_eArgError,
        "wrong number of arguments (%d for %d)", argc, ptr->dim);
  }
  return Qnil;
}
static VALUE rb_table_set(int argc, VALUE *argv, VALUE self) {
  Table *ptr = convertTable(self);
  if(argc == ptr->dim+1) {
    ptr->set_internal(
        argc,
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

static int get_int(const char *ptr) {
  return
    ((uint32_t)(unsigned char)ptr[0])|
    ((uint32_t)(unsigned char)ptr[1]<<8)|
    ((uint32_t)(unsigned char)ptr[2]<<16)|
    ((uint32_t)(unsigned char)ptr[3]<<24);
}
static short get_short(const char *ptr) {
  return
    ((uint32_t)(unsigned char)ptr[0])|
    ((uint32_t)(unsigned char)ptr[1]<<8);
}

static VALUE rb_table_old_load(VALUE, VALUE str) {
  VALUE ret = table_alloc(rb_cTable);
  Table *ptr = convertTable(ret);
  ptr->data = nullptr;
  char *s = StringValuePtr(str);
  if(!s) return ret;
  // TODO make it more safe
  ptr->dim = get_int(s);
  ptr->xsize = get_int(s+4);
  ptr->ysize = get_int(s+8);
  ptr->zsize = get_int(s+12);
  int size = get_int(s+16);
  ptr->data = new short[size];
  for(int i = 0; i < size; ++i) {
    ptr->data[i] = get_short(s+20+i*2);
  }
  return ret;
}

static void put_int(char *ptr, uint32_t val) {
  ptr[0] = val;
  ptr[1] = val>>8;
  ptr[2] = val>>16;
  ptr[3] = val>>24;
}
static void put_short(char *ptr, uint32_t val) {
  ptr[0] = val;
  ptr[1] = val>>8;
}
static VALUE rb_table_old_dump(VALUE self, VALUE) {
  Table *ptr = convertTable(self);
  int size = ptr->xsize * ptr->ysize * ptr->zsize;
  char *s = new char[size];
  put_int(s, ptr->dim);
  put_int(s+4, ptr->xsize);
  put_int(s+8, ptr->ysize);
  put_int(s+12, ptr->zsize);
  put_int(s+16, size);
  for(int i = 0; i < size; ++i) {
    put_short(s+20+i*2, ptr->data[i]);
  }
  VALUE ret = rb_str_new(s, size);
  delete[] s;
  return ret;
}
