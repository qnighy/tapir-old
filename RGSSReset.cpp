#include "RGSSReset.h"

VALUE rb_eRGSSReset;
void InitRGSSReset() {
  rb_eRGSSReset = rb_define_class("RGSSReset", rb_eException);
}
