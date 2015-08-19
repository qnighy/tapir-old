#include <stdint.h>

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
