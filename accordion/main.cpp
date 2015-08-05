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

RUBY_GLOBAL_SETUP

// int main(int argc, char *argv[]) {
int main(int, char **) {
  initSDL();

  int ruby_argc = 2;
  char *ruby_argv_array[] = {
    (char*)"ruby",
    (char*)"/home/qnighy/workdir/vxace/ext/main.rb",
    NULL
  };
  char **ruby_argv = ruby_argv_array;
  ruby_sysinit(&ruby_argc, &ruby_argv);
  {
    RUBY_INIT_STACK;
    ruby_init();

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

    ruby_run_node(ruby_options(ruby_argc, ruby_argv));
  }
  printf("Done.\n");
  quitSDL();
  return 0;
}
