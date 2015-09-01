#include "RGSSReset.h"

VALUE rb_eRGSSReset;

/*
 * Raised when reset key is pressed.
 */
void Init_RGSSReset(void) {
  rb_eRGSSReset = rb_define_class("RGSSReset", rb_eException);
}
