#!/usr/bin/make -f

PC_LIBS = sdl2 SDL2_image SDL2_ttf
RUBY_SRC_DIR = $(shell readlink -f ../ruby192)
RUBY_DIR = $(shell readlink -f ../ruby192-build)

TEST_GAME_DIR = ../games/Sakusaku

CC = g++
CFLAGS = -std=c11 -O2 -Wall -Wextra -g \
	 $(shell pkg-config $(PC_LIBS) --cflags-only-other)
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -g
CPPFLAGS = \
	   -I$(RUBY_DIR)/include/ruby-1.9.1/x86_64-linux \
	   -I$(RUBY_DIR)/include/ruby-1.9.1 \
	   $(shell pkg-config $(PC_LIBS) --cflags-only-I)
LDFLAGS = \
	  -L$(RUBY_DIR)/lib \
	  $(shell pkg-config $(PC_LIBS) --libs-only-L --libs-only-other)
LDLIBS = \
	 $(RUBY_SRC_DIR)/ext/zlib/zlib.a -lz \
	 -lruby-static -lpthread -lrt -ldl -lcrypt -lm \
	 $(shell pkg-config $(PC_LIBS) --libs-only-l)

EXEC = tapir
OBJS = main.o main_rb.o misc.o file_misc.o sdl_misc.o renderable.o rpg_rb.o \
       Bitmap.o Color.o Font.o Rect.o Sprite.o \
       Table.o Tilemap.o Tone.o Viewport.o \
       Window.o Graphics.o Input.o RGSSReset.o

.PHONY: all clean run run-valgrind

all: $(EXEC)

clean:
	$(RM) $(EXEC) $(OBJS) $(wildcard *.d)

run: all
	cp $(EXEC) $(TEST_GAME_DIR)
	cd $(TEST_GAME_DIR); ./$(EXEC)

run-valgrind: all
	cp $(EXEC) $(TEST_GAME_DIR)
	cd $(TEST_GAME_DIR); valgrind --undef-value-errors=no ./$(EXEC)

run-gdb: all
	cp $(EXEC) $(TEST_GAME_DIR)
	cd $(TEST_GAME_DIR); gdb -ex='dir ../../accordion' -ex='dir ../../ruby192' ./$(EXEC)

tapir: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) -MMD $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

main_rb.cpp: main_gen.rb main.rb
	ruby main_gen.rb main.rb > $@

-include $(wildcard *.d)
