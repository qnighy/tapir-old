#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <ruby.h>

constexpr int RGSS_DOWN = 2;
constexpr int RGSS_LEFT = 4;
constexpr int RGSS_RIGHT = 6;
constexpr int RGSS_UP = 8;
constexpr int RGSS_A = 11;
constexpr int RGSS_B = 12;
constexpr int RGSS_C = 13;
constexpr int RGSS_X = 14;
constexpr int RGSS_Y = 15;
constexpr int RGSS_Z = 16;
constexpr int RGSS_L = 17;
constexpr int RGSS_R = 18;
constexpr int RGSS_SHIFT = 21;
constexpr int RGSS_CTRL = 22;
constexpr int RGSS_ALT = 23;
constexpr int RGSS_F5 = 25;
constexpr int RGSS_F6 = 26;
constexpr int RGSS_F7 = 27;
constexpr int RGSS_F8 = 28;
constexpr int RGSS_F9 = 29;

extern VALUE rb_mInput;
extern void InitInput();

VALUE rb_input_update(VALUE self);
VALUE rb_input_press_p(VALUE self, VALUE sym);
VALUE rb_input_trigger_p(VALUE self, VALUE sym);
VALUE rb_input_repeat_p(VALUE self, VALUE sym);
VALUE rb_input_dir4(VALUE self);
VALUE rb_input_dir8(VALUE self);

#endif // INPUT_H_INCLUDED
