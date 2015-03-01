#ifndef RENDERABLE_H_INCLUDED
#define RENDERABLE_H_INCLUDED

#include <cstdint>
#include <vector>

extern uint64_t current_renderable_id;
enum class RenderableType {
  NONE, SPRITE, WINDOW, PLANE, TILEMAP, VIEWPORT
};
struct Renderable {
  RenderableType type;
  int z, y;
  uint64_t renderable_id;
};
extern std::vector<Renderable*> *global_renderables;
#endif // RENDERABLE_H_INCLUDED
