#ifndef TONE_H_INCLUDED
#define TONE_H_INCLUDED

#include <ruby.h>

struct Tone {
  VALUE rb_parent;
  double red, green, blue, gray;
  void initialize(double red, double green, double blue, double gray = 0.0);
  void initialize();
  void set(double red, double green, double blue, double gray = 0.0);
  void set(Tone *tone);
};

extern VALUE rb_cTone;
extern void InitTone();

Tone *convertTone(VALUE obj);
Tone *convertToneOrNil(VALUE obj);

Tone *newTone(double red, double green, double blue, double gray = 0.0);
Tone *newTone();

#endif // TONE_H_INCLUDED
