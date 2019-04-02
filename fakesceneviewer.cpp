#include "fakesceneviewer.hpp"

#include "viewportrect.hpp"
#include "sceneviewerimpl.hpp"


ViewportPoint FakeSceneViewer::centerOfBody(const Scene::Body &desired_body)
{
  FakeSceneViewer &scene_viewer = *this;
  assert(scene_viewer.scenePtr());
  const Scene &scene = *scene_viewer.scenePtr();
  Optional<ViewportRect> maybe_matching_rect;

  auto match = [&](const Scene::Body &body,const ViewportRect &rect)
  {
    if (&body == &desired_body) {
      maybe_matching_rect = rect;
    }
  };

  scene_viewer::forEachSceneBodyRect(scene,match);

  assert(maybe_matching_rect);

  return maybe_matching_rect->center();
}
