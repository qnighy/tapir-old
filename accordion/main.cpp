#include <ruby.h>
#include <SDL.h>
#include <cstdio>

#include "Bitmap.h"
#include "Color.h"
#include "Font.h"
#include "Rect.h"
#include "Sprite.h"
#include "Table.h"
#include "Tilemap.h"
#include "Tone.h"
#include "Viewport.h"
#include "Window.h"
#include "Graphics.h"
#include "Input.h"
#include "RGSSReset.h"
#include "sdl_misc.h"
#include "file_misc.h"
#include "rpg_rb.h"

extern "C" void Init_zlib(void);

RUBY_GLOBAL_SETUP

static VALUE rescued_main(VALUE) {
  rb_eval_string("load '/home/qnighy/workdir/tapir/accordion/main.rb'");
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
    InitRPG();

    InitBitmap();
    InitColor();
    InitFont();
    InitRect();
    InitSprite();
    InitTable();
    InitTilemap();
    InitTone();
    InitViewport();
    InitWindow();
    InitGraphics();
    InitInput();
    InitRGSSReset();

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
