#include "sceneviewer.hpp"

#include <iostream>
#include "scene.hpp"
#include "viewportrect.hpp"
#include "sceneviewerimpl.hpp"

using std::cerr;
using std::ostream;
using std::vector;


void
  SceneViewer::setScenePtr(
    Scene *scene_ptr_arg,
    SceneListener *listener_ptr_arg
  )
{
  scene_ptr = scene_ptr_arg;
  listener_ptr = listener_ptr_arg;
}


static Point2D
  bodyGlobalPosition(
    const Scene::Body &body,
    const Scene::Frame &frame
  )
{
  return globalPos(body,/*local*/Point2D(0,0),frame);
}


static Point2D
  parentGlobalPosition(const Scene::Body &body,const Scene::Frame &frame)
{
  Scene::Body *parent_ptr = body.parentPtr();

  if (parent_ptr) {
    return bodyGlobalPosition(*parent_ptr,frame);
  }
  else {
    assert(false);
  }
}


static void
  setBodyGlobalPosition(
    Scene::Body &body,
    Scene::Frame &frame,
    const Point2D &new_global_position
  )
{
  Point2D parent_global_position = parentGlobalPosition(body,frame);

  Vector2D new_local_position =
    new_global_position - parent_global_position;
  Point2D v{new_local_position.x,new_local_position.y};
  setBodyPosition(body,frame,v);
}


void SceneViewer::mousePressedAt(const ViewportPoint &p)
{
  using Body = Scene::Body;
  maybe_mouse_click_down_position = p;

  if (!scene_ptr) return;

  const Scene &scene = *scene_ptr;
  const Body *body_ptr = 0;

  auto check = [&](const Body &body,const ViewportRect &rect){
    if (rect.contains(p)) {
      body_ptr = &body;
    }
  };

  scene_viewer::forEachSceneBodyRect(scene,check);

  if (!body_ptr) {
    cerr << "No body found\n";
  }
  else {
    cerr << "found body " << body_ptr->name << "\n";
    clicked_on_body_ptr = const_cast<Body*>(body_ptr);
    const Body &body = *body_ptr;
    maybe_body_click_down_position =
      bodyGlobalPosition(body,scene.displayFrame());
  }
}


#if 0
static ostream& operator<<(ostream &stream,const ViewportPoint &p)
{
  stream << "ViewportPoint(" << p.x << "," << p.y << ")";
  return stream;
}
#endif


void SceneViewer::mouseMovedTo(const ViewportPoint &p)
{
  if (!clicked_on_body_ptr) {
    return;
  }

  // If we were able to click on a body, we must have had a scene.
  assert(scene_ptr);
  Scene &scene = *scene_ptr;
  Scene::Body &clicked_on_body = *clicked_on_body_ptr;

  if (!maybe_mouse_click_down_position) {
    assert(false);
  }

  const ViewportPoint &mouse_click_down_position =
    *maybe_mouse_click_down_position;

  assert(maybe_body_click_down_position);
    // If we have a selected body, then we should have a body click-down
    // position.

  const Point2D &body_click_down_position = *maybe_body_click_down_position;

  ViewportVector delta = p - mouse_click_down_position;
  Vector2D offset{delta.x,delta.y};
  Point2D new_position = body_click_down_position + offset;
  setBodyGlobalPosition(clicked_on_body,scene.backgroundFrame(),new_position);

  vector<int> variable_indices;
  variable_indices.push_back(clicked_on_body.position_map.x.var_index);
  variable_indices.push_back(clicked_on_body.position_map.y.var_index);

  assert(listener_ptr);
  listener_ptr->frameVariablesChanged(
    scene.currentFrameIndex(),
    variable_indices
  );
}


void SceneViewer::mouseReleased()
{
  maybe_mouse_click_down_position.reset();

  clicked_on_body_ptr = nullptr;
}
