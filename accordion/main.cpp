#include <ruby.h>
#include <SDL.h>
#include <cstdio>

#include "file_misc.h"
#include "sdl_misc.h"
#include "rpg_rb.h"
#include "main_rb.h"
#include "RGSSError.h"
#include "RGSSReset.h"
#include "Color.h"
#include "Tone.h"
#include "Rect.h"
#include "Table.h"
#include "Font.h"
#include "Bitmap.h"
#include "Graphics.h"
#include "Sprite.h"
#include "Tilemap.h"
#include "Viewport.h"
#include "Window.h"
#include "Input.h"

extern "C" void Init_zlib(void);

RUBY_GLOBAL_SETUP

static VALUE rescued_main(VALUE) {
  Init_main();
  return Qnil;
}

static VALUE protected_main(VALUE);

// int main(int argc, char *argv[]) {
int main(int, char **) {
  initSDL();

  int ruby_argc = 2;
  char *ruby_argv_array[] = {
    (char*)"ruby",
    NULL
  };
  char **ruby_argv = ruby_argv_array;
  ruby_sysinit(&ruby_argc, &ruby_argv);
  {
    RUBY_INIT_STACK;
    ruby_init();

    Init_zlib();

    InitFileMisc();

    Init_RGSSError();
    Init_RGSSReset();
    Init_Color();
    Init_Tone();
    Init_Rect();
    InitTable();
    InitFont();
    InitBitmap();
    InitGraphics();
    InitSprite();
    InitTilemap();
    InitViewport();
    InitWindow();
    InitInput();
    InitRPG();

    rb_protect(protected_main, Qnil, NULL);
  }
  printf("Done.\n");
  quitSDL();
  return 0;
}

struct print_bt_context {
  int count;
  VALUE err_message;
  VALUE err_class;
};

static VALUE print_bt_each(VALUE btentry, VALUE data, VALUE) {
  print_bt_context *ctx = (print_bt_context*) data;
  if(ctx->count++ == 0) {
    printf("%s: %s (%s)\n",
        StringValueCStr(btentry),
        StringValueCStr(ctx->err_message),
        StringValueCStr(ctx->err_class));
  } else {
    printf("\tfrom %s\n", StringValueCStr(btentry));
  }
  return Qnil;
}

static VALUE print_bt(VALUE err_backtrace) {
  ID id_each;
  CONST_ID(id_each, "each");
  rb_funcall(err_backtrace, id_each, 0);
  return Qnil;
}

static VALUE rescued_main_rescue(VALUE, VALUE errinfo) {
  ID id_message, id_backtrace;
  CONST_ID(id_message, "message");
  CONST_ID(id_backtrace, "backtrace");

  print_bt_context ctx;
  ctx.count = 0;
  ctx.err_message = rb_funcall(errinfo, id_message, 0);
  VALUE err_backtrace = rb_funcall(errinfo, id_backtrace, 0);
  ctx.err_class = rb_mod_name(rb_obj_class(errinfo));
  rb_iterate(
      print_bt, err_backtrace,
      (VALUE(*)(...))print_bt_each, (VALUE) &ctx);
  return Qnil;
}

static VALUE protected_main(VALUE) {
  rb_rescue2(
      (VALUE(*)(...))rescued_main, Qnil,
      (VALUE(*)(...))rescued_main_rescue, Qnil,
      rb_eException, NULL);
  return Qnil;
}
