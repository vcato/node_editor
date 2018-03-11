#include "scenetree.hpp"

#include <sstream>


using Body = Scene::Body;
using std::cerr;
using std::ostringstream;


void SceneTree::notifySceneChanged()
{
  updateItems();
}


static SceneTree::Item &addItemTo(SceneTree::Item &parent_item)
{
  parent_item.children.emplace_back();
  return parent_item.children.back();
}


static void
  addBodiesTo(
    SceneTree::Item &parent_item,
    const Scene::Bodies &bodies,
    const Scene::Frame &frame
  )
{
  for (auto &body : bodies) {
    auto &new_body_item = addItemTo(parent_item);
    new_body_item.label = body.name;
    auto &new_position_item = addItemTo(new_body_item);
    new_position_item.label = "Position:";
    {
      auto &new_x_item = addItemTo(new_position_item);
      ostringstream stream;
      stream << "X: " << body.position.x(frame);
      new_x_item.label = stream.str();
    }
    {
      auto &new_y_item = addItemTo(new_position_item);
      ostringstream stream;
      stream << "Y: " << body.position.y(frame);
      new_y_item.label = stream.str();
    }
    addBodiesTo(new_body_item,body.children,frame);
  }
}


static SceneTree::Item makeItems(const Scene *scene_ptr)
{
  SceneTree::Item root;

  if (!scene_ptr) {
    return root;
  }

  addBodiesTo(root,scene_ptr->bodies(),scene_ptr->displayFrame());

  return root;
}


void SceneTree::setScenePtr(Scene *arg)
{
  scene_ptr = arg;
  updateItems();
}


void SceneTree::updateItems()
{
  setItems(makeItems(scene_ptr));
}
