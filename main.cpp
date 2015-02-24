#include <ruby.h>
#include <cstdio>

RUBY_GLOBAL_SETUP

int main(int argc, char *argv[]) {
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
    // ruby_init_loadpath();

    ruby_run_node(ruby_options(ruby_argc, ruby_argv));
  }
  printf("Done.\n");
  return 0;
}
