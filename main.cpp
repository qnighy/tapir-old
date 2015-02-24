#include <ruby.h>
#include <SDL.h>
#include <cstdio>

#include "Rect.h"
#include "Graphics.h"
#include "sdl_misc.h"

RUBY_GLOBAL_SETUP

int main(int argc, char *argv[]) {
  initSDL();

  int ruby_argc = 2;
  char *ruby_argv_array[] = {
    (char*)"ruby",
    (char*)"main.rb",
    NULL
  };
  char **ruby_argv = ruby_argv_array;
  ruby_sysinit(&ruby_argc, &ruby_argv);
  {
    RUBY_INIT_STACK;
    ruby_init();

    InitRect();
    InitGraphics();

    ruby_run_node(ruby_options(ruby_argc, ruby_argv));
  }
  printf("Done.\n");
  quitSDL();
  return 0;
}
