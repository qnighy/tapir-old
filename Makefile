#!/usr/bin/make -f

PC_LIBS = ruby-1.9 sdl2 SDL2_image

CC = g++
CFLAGS = -std=c11 -O2 -Wall -Wextra -g \
	 $(shell pkg-config $(PC_LIBS) --cflags-only-other)
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -g
CPPFLAGS = $(shell pkg-config $(PC_LIBS) --cflags-only-I)
LDFLAGS = $(shell pkg-config $(PC_LIBS) --libs-only-L --libs-only-other)
LDLIBS = $(shell pkg-config $(PC_LIBS) --libs-only-l)

EXEC = main
# OBJS = main.o \
#        Bitmap.o Color.o Font.o Plane.o Rect.o Sprite.o \
#        Table.o Tilemap.o Tone.o Viewport.o Window.o
OBJS = main.o misc.o sdl_misc.o \
       Bitmap.o Color.o Font.o Rect.o Sprite.o Tone.o Viewport.o Graphics.o

all: $(EXEC)

clean:
	$(RM) $(EXEC) $(OBJS) $(wildcard *.d)

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) -MMD $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

-include $(wildcard *.d)
