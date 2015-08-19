#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

double readDouble(const char *ptr);
void writeDouble(char *ptr, double val);

inline int saturateInt(int val, int minval, int maxval) {
  if(val <= minval) return minval;
  if(val >= maxval) return maxval;
  return val;
}

inline int saturateDouble(double val, double minval, double maxval) {
  if(val <= minval) return minval;
  if(val >= maxval) return maxval;
  return val;
}

#ifdef __cplusplus
}
#endif

#endif // MISC_H_INCLUDED
