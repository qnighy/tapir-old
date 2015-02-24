#include <algorithm>

#include <ruby.h>
#include <SDL.h>

#include "Sprite.h"
#include "Viewport.h"
#include "Graphics.h"
#include "sdl_misc.h"
#include "renderable.h"

uint64_t current_renderable_id;
std::vector<Renderable*> *global_renderables;

void register_renderable(Renderable *r, Viewport *v) {
  fprintf(stderr, "Renderable Registered\n");
  std::vector<Renderable*> *rs = v ? v->renderables : global_renderables;
  rs->push_back(r);
}
void unregister_renderable(Renderable *r, Viewport *v) {
  fprintf(stderr, "Renderable Unregistered\n");
  std::vector<Renderable*> *rs = v ? v->renderables : global_renderables;
  rs->resize(std::remove(rs->begin(), rs->end(), r) - rs->begin());
}
void sort_renderables(std::vector<Renderable*> *rs) {
  std::sort(rs->begin(), rs->end(), [](Renderable *a, Renderable *b) {
      if(a->z != b->z) return a->z < b->z;
      if(a->y != b->y) return a->y < b->y;
      return a->renderable_id < b->renderable_id;
  });
}
void render_renderable(Renderable *r, SDL_Renderer *renderer) {
  switch(r->type) {
    case RenderableType::SPRITE:
      ((Sprite*)r)->render(renderer);
      break;
    case RenderableType::VIEWPORT:
      ((Viewport*)r)->render(renderer);
      break;
    default:
      break;
  }
}

VALUE rb_mGraphics;

void InitGraphics() {
  current_renderable_id = 0;
  global_renderables = new std::vector<Renderable*>();

  rb_mGraphics = rb_define_module("Graphics");
  rb_define_singleton_method(rb_mGraphics, "update",
      (VALUE(*)(...))rb_graphics_update, 0);
}

VALUE rb_graphics_update(VALUE self) {
  pollEvent();
  SDL_SetRenderDrawBlendMode(mainWindowRenderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(mainWindowRenderer, 0, 0, 0, 255);
  SDL_RenderFillRect(mainWindowRenderer, NULL);
  sort_renderables(global_renderables);
  for(Renderable *r : *global_renderables) {
    render_renderable(r, mainWindowRenderer);
  }
  SDL_RenderPresent(mainWindowRenderer);
  SDL_Delay(1000/60);
  return Qnil;
}
