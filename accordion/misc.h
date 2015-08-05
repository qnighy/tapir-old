#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

double readDouble(const char *ptr);
void writeDouble(char *ptr, double val);

template<typename T>
T saturate(T val, T minval, T maxval) {
  if(val <= minval) return minval;
  if(val >= maxval) return maxval;
  return val;
}

#endif // MISC_H_INCLUDED
