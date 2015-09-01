#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

double readDouble(const char *ptr);
void writeDouble(char *ptr, double val);
int32_t readInt32(const char *ptr);
void writeInt32(char *ptr, int32_t val);
int16_t readInt16(const char *ptr);
void writeInt16(char *ptr, int16_t val);

inline int16_t wrapToInt16(int32_t val) {
  union {
    uint16_t u16;
    int16_t i16;
  } num;
  num.u16 = val;
  return num.i16;
}

inline int32_t saturateInt32(int32_t val, int32_t minval, int32_t maxval) {
  if(val <= minval) return minval;
  if(val >= maxval) return maxval;
  return val;
}

inline double saturateDouble(double val, double minval, double maxval) {
  if(val <= minval) return minval;
  if(val >= maxval) return maxval;
  return val;
}

#ifdef __cplusplus
}
#endif

#endif // MISC_H_INCLUDED
