#include "scenetree.hpp"


using Body = Scene::Body;

void SceneTree::notifySceneChanged()
{
  updateItems();
}


static void
  addBodiesTo(SceneTree::Item &parent_item,const Scene::Bodies &bodies)
{
  for (auto &body : bodies) {
    parent_item.children.emplace_back();
    parent_item.children.back().label = body.name;
    addBodiesTo(parent_item.children.back(),body.children);
  }
}


static SceneTree::Item makeItems(const Scene *scene_ptr)
{
  SceneTree::Item root;

  if (!scene_ptr) {
    return root;
  }

  addBodiesTo(root,scene_ptr->bodies());

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
