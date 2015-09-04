#include "misc.h"

union u64d_converter {
  uint64_t u64;
  double d;
};

double readDouble(const char *ptr) {
  union u64d_converter num;
  num.u64 =
    ((uint64_t)(unsigned char)ptr[0])|
    ((uint64_t)(unsigned char)ptr[1]<<8)|
    ((uint64_t)(unsigned char)ptr[2]<<16)|
    ((uint64_t)(unsigned char)ptr[3]<<24)|
    ((uint64_t)(unsigned char)ptr[4]<<32)|
    ((uint64_t)(unsigned char)ptr[5]<<40)|
    ((uint64_t)(unsigned char)ptr[6]<<48)|
    ((uint64_t)(unsigned char)ptr[7]<<56);
  return num.d;
}
void writeDouble(char *ptr, double val) {
  union u64d_converter num;
  num.d = val;
  ptr[0] = num.u64;
  ptr[1] = num.u64>>8;
  ptr[2] = num.u64>>16;
  ptr[3] = num.u64>>24;
  ptr[4] = num.u64>>32;
  ptr[5] = num.u64>>40;
  ptr[6] = num.u64>>48;
  ptr[7] = num.u64>>56;
}

union u32i_converter {
  uint32_t u32;
  int32_t i32;
};

int32_t readInt32(const char *ptr) {
  union u32i_converter num;
  num.u32 =
    ((uint32_t)(unsigned char)ptr[0])|
    ((uint32_t)(unsigned char)ptr[1]<<8)|
    ((uint32_t)(unsigned char)ptr[2]<<16)|
    ((uint32_t)(unsigned char)ptr[3]<<24);
  return num.i32;
}
void writeInt32(char *ptr, int32_t val) {
  union u32i_converter num;
  num.i32 = val;
  ptr[0] = num.u32;
  ptr[1] = num.u32>>8;
  ptr[2] = num.u32>>16;
  ptr[3] = num.u32>>24;
}

union u16i_converter {
  uint16_t u16;
  int16_t i16;
};

int16_t readInt16(const char *ptr) {
  union u16i_converter num;
  num.u16 =
    ((uint16_t)(unsigned char)ptr[0])|
    ((uint16_t)(unsigned char)ptr[1]<<8);
  return num.i16;
}
void writeInt16(char *ptr, int16_t val) {
  union u16i_converter num;
  num.i16 = val;
  ptr[0] = num.u16;
  ptr[1] = num.u16>>8;
}
