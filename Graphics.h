#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <ruby.h>

#include "renderable.h"
#include "Viewport.h"

void register_renderable(Renderable *r, Viewport *v);
void unregister_renderable(Renderable *r, Viewport *v);

void sort_renderables(std::vector<Renderable*> *rs);
void render_renderable(Renderable *r, SDL_Renderer *renderer);

extern VALUE rb_mGraphics;
extern void InitGraphics();

VALUE rb_graphics_update(VALUE self);

#endif // GRAPHICS_H_INCLUDED
