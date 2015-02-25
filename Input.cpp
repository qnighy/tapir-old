#include <algorithm>

#include <ruby.h>
#include <SDL.h>

#include "Input.h"
#include "sdl_misc.h"

static VALUE sym_DOWN;
static VALUE sym_LEFT;
static VALUE sym_RIGHT;
static VALUE sym_UP;
static VALUE sym_A;
static VALUE sym_B;
static VALUE sym_C;
static VALUE sym_X;
static VALUE sym_Y;
static VALUE sym_Z;
static VALUE sym_L;
static VALUE sym_R;
static VALUE sym_SHIFT;
static VALUE sym_CTRL;
static VALUE sym_ALT;
static VALUE sym_F5;
static VALUE sym_F6;
static VALUE sym_F7;
static VALUE sym_F8;
static VALUE sym_F9;

VALUE rb_mInput;

void InitInput() {
  rb_mInput = rb_define_module("Input");
  rb_define_singleton_method(rb_mInput, "update",
      (VALUE(*)(...))rb_input_update, 0);
  rb_define_singleton_method(rb_mInput, "press?",
      (VALUE(*)(...))rb_input_press_p, 1);
  rb_define_singleton_method(rb_mInput, "trigger?",
      (VALUE(*)(...))rb_input_trigger_p, 1);
  rb_define_singleton_method(rb_mInput, "repeat?",
      (VALUE(*)(...))rb_input_repeat_p, 1);
  rb_define_singleton_method(rb_mInput, "dir4",
      (VALUE(*)(...))rb_input_dir4, 0);
  rb_define_singleton_method(rb_mInput, "dir8",
      (VALUE(*)(...))rb_input_dir8, 0);

  sym_DOWN = ID2SYM(rb_intern("DOWN"));
  sym_LEFT = ID2SYM(rb_intern("LEFT"));
  sym_RIGHT = ID2SYM(rb_intern("RIGHT"));
  sym_UP = ID2SYM(rb_intern("UP"));
  sym_A = ID2SYM(rb_intern("A"));
  sym_B = ID2SYM(rb_intern("B"));
  sym_C = ID2SYM(rb_intern("C"));
  sym_X = ID2SYM(rb_intern("X"));
  sym_Y = ID2SYM(rb_intern("Y"));
  sym_Z = ID2SYM(rb_intern("Z"));
  sym_L = ID2SYM(rb_intern("L"));
  sym_R = ID2SYM(rb_intern("R"));
  sym_SHIFT = ID2SYM(rb_intern("SHIFT"));
  sym_CTRL = ID2SYM(rb_intern("CTRL"));
  sym_ALT = ID2SYM(rb_intern("ALT"));
  sym_F5 = ID2SYM(rb_intern("F5"));
  sym_F6 = ID2SYM(rb_intern("F6"));
  sym_F7 = ID2SYM(rb_intern("F7"));
  sym_F8 = ID2SYM(rb_intern("F8"));
  sym_F9 = ID2SYM(rb_intern("F9"));
}

static bool buttonstates[30];
static int buttoncounts[30];
static int dir4, dir8;

static const int kbdassign[15] = {
  RGSS_C, RGSS_C, RGSS_B, RGSS_B, RGSS_A,
  RGSS_C, RGSS_B, -1, -1, -1,
  RGSS_X, RGSS_Y, RGSS_Z, RGSS_L, RGSS_R
};

VALUE rb_input_update(VALUE self) {
  pollEvent();
  const Uint8 *state = SDL_GetKeyboardState(nullptr);
  bool kbdstates[15];
  kbdstates[0] = state[SDL_SCANCODE_SPACE];
  kbdstates[1] = state[SDL_SCANCODE_RETURN] || state[SDL_SCANCODE_RETURN2];
  kbdstates[2] = state[SDL_SCANCODE_ESCAPE];
  kbdstates[3] = state[SDL_SCANCODE_KP_0];
  kbdstates[4] = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
  kbdstates[5] = state[SDL_SCANCODE_Z];
  kbdstates[6] = state[SDL_SCANCODE_X];
  kbdstates[7] = state[SDL_SCANCODE_C];
  kbdstates[8] = state[SDL_SCANCODE_V];
  kbdstates[9] = state[SDL_SCANCODE_B];
  kbdstates[10] = state[SDL_SCANCODE_A];
  kbdstates[11] = state[SDL_SCANCODE_S];
  kbdstates[12] = state[SDL_SCANCODE_D];
  kbdstates[13] = state[SDL_SCANCODE_Q];
  kbdstates[14] = state[SDL_SCANCODE_W];
  for(int i = 0; i < 30; ++i) {
    buttonstates[i] = false;
  }
  for(int i = 0; i < 15; ++i) {
    if(kbdassign[i] != -1 && kbdstates[i]) {
      buttonstates[kbdassign[i]] = true;
    }
  }
  buttonstates[RGSS_DOWN] |= state[SDL_SCANCODE_DOWN];
  buttonstates[RGSS_LEFT] |= state[SDL_SCANCODE_LEFT];
  buttonstates[RGSS_RIGHT] |= state[SDL_SCANCODE_RIGHT];
  buttonstates[RGSS_UP] |= state[SDL_SCANCODE_UP];
  buttonstates[RGSS_SHIFT] |=
    state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
  buttonstates[RGSS_CTRL] |=
    state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL];
  buttonstates[RGSS_ALT] |=
    state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT];
  buttonstates[RGSS_F5] |= state[SDL_SCANCODE_F5];
  buttonstates[RGSS_F6] |= state[SDL_SCANCODE_F6];
  buttonstates[RGSS_F7] |= state[SDL_SCANCODE_F7];
  buttonstates[RGSS_F8] |= state[SDL_SCANCODE_F8];
  buttonstates[RGSS_F9] |= state[SDL_SCANCODE_F9];
  for(int i = 0; i < 30; ++i) {
    if(buttonstates[i]) {
      ++buttoncounts[i];
    } else {
      buttoncounts[i] = 0;
    }
  }

  dir8 = 5 +
    (buttonstates[RGSS_DOWN] ? -3 : 0) +
    (buttonstates[RGSS_LEFT] ? -1 : 0) +
    (buttonstates[RGSS_RIGHT] ? 1 : 0) +
    (buttonstates[RGSS_UP] ? 3 : 0);
  if(dir8==5) dir8=0;

  dir4 = dir8;
  if(dir4 == 1) {
    dir4 = buttoncounts[RGSS_DOWN] > buttoncounts[RGSS_LEFT] ? 2 : 4;
  } else if(dir4 == 3) {
    dir4 = buttoncounts[RGSS_DOWN] > buttoncounts[RGSS_RIGHT] ? 2 : 6;
  } else if(dir4 == 7) {
    dir4 = buttoncounts[RGSS_UP] > buttoncounts[RGSS_LEFT] ? 8 : 4;
  } else if(dir4 == 9) {
    dir4 = buttoncounts[RGSS_UP] > buttoncounts[RGSS_RIGHT] ? 8 : 6;
  }

  return Qnil;
}

static int convert_sym(VALUE sym) {
  if(sym == sym_DOWN) return RGSS_DOWN;
  if(sym == sym_LEFT) return RGSS_LEFT;
  if(sym == sym_RIGHT) return RGSS_RIGHT;
  if(sym == sym_UP) return RGSS_UP;
  if(sym == sym_A) return RGSS_A;
  if(sym == sym_B) return RGSS_B;
  if(sym == sym_C) return RGSS_C;
  if(sym == sym_X) return RGSS_X;
  if(sym == sym_Y) return RGSS_Y;
  if(sym == sym_Z) return RGSS_Z;
  if(sym == sym_L) return RGSS_L;
  if(sym == sym_R) return RGSS_R;
  if(sym == sym_SHIFT) return RGSS_SHIFT;
  if(sym == sym_CTRL) return RGSS_CTRL;
  if(sym == sym_ALT) return RGSS_ALT;
  if(sym == sym_F5) return RGSS_F5;
  if(sym == sym_F6) return RGSS_F6;
  if(sym == sym_F7) return RGSS_F7;
  if(sym == sym_F8) return RGSS_F8;
  if(sym == sym_F9) return RGSS_F9;
  if(sym == INT2FIX(RGSS_DOWN)) return RGSS_DOWN;
  if(sym == INT2FIX(RGSS_LEFT)) return RGSS_LEFT;
  if(sym == INT2FIX(RGSS_RIGHT)) return RGSS_RIGHT;
  if(sym == INT2FIX(RGSS_UP)) return RGSS_UP;
  if(sym == INT2FIX(RGSS_A)) return RGSS_A;
  if(sym == INT2FIX(RGSS_B)) return RGSS_B;
  if(sym == INT2FIX(RGSS_C)) return RGSS_C;
  if(sym == INT2FIX(RGSS_X)) return RGSS_X;
  if(sym == INT2FIX(RGSS_Y)) return RGSS_Y;
  if(sym == INT2FIX(RGSS_Z)) return RGSS_Z;
  if(sym == INT2FIX(RGSS_L)) return RGSS_L;
  if(sym == INT2FIX(RGSS_R)) return RGSS_R;
  if(sym == INT2FIX(RGSS_SHIFT)) return RGSS_SHIFT;
  if(sym == INT2FIX(RGSS_CTRL)) return RGSS_CTRL;
  if(sym == INT2FIX(RGSS_ALT)) return RGSS_ALT;
  if(sym == INT2FIX(RGSS_F5)) return RGSS_F5;
  if(sym == INT2FIX(RGSS_F6)) return RGSS_F6;
  if(sym == INT2FIX(RGSS_F7)) return RGSS_F7;
  if(sym == INT2FIX(RGSS_F8)) return RGSS_F8;
  if(sym == INT2FIX(RGSS_F9)) return RGSS_F9;
  return 0;
}
VALUE rb_input_press_p(VALUE, VALUE sym) {
  return buttonstates[convert_sym(sym)] ? Qtrue : Qfalse;
}
VALUE rb_input_trigger_p(VALUE, VALUE sym) {
  int cnt = buttoncounts[convert_sym(sym)];
  return (cnt == 1) ? Qtrue : Qfalse;
}
VALUE rb_input_repeat_p(VALUE, VALUE sym) {
  int cnt = buttoncounts[convert_sym(sym)];
  return (cnt == 1 || (cnt >= 24 && cnt%6 == 0)) ? Qtrue : Qfalse;
}
VALUE rb_input_dir4(VALUE) {
  return INT2NUM(dir4);
}
VALUE rb_input_dir8(VALUE) {
  return INT2NUM(dir8);
}
