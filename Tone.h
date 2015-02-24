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

  static Tone *create(double red, double green, double blue, double gray = 0.0);
  static Tone *create();
};

extern VALUE rb_cTone;
extern void InitTone();

Tone *convertTone(VALUE obj);
Tone *convertToneOrNil(VALUE obj);
VALUE exportTone(Tone *ptr);

#endif // TONE_H_INCLUDED
