#include "scenetree.hpp"

#include <sstream>
#include <vector>


using Body = Scene::Body;
using std::cerr;
using std::ostringstream;
using std::vector;
using Item = SceneTree::ItemData;


void SceneTree::notifySceneChanged()
{
  updateItems();
}


static Item &addItemTo(Item &parent_item)
{
  parent_item.children.emplace_back();
  return parent_item.children.back();
}


static void
  addBodiesTo(
    Item &parent_item,
    const Scene::Bodies &bodies,
    const Scene::Frame &frame
  );


static void
  buildBodyItem(
    Item &new_body_item,
    const Scene::Body &body,
    const Scene::Frame &frame
  )
{
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


static void
  addBodiesTo(
    Item &parent_item,
    const Scene::Bodies &bodies,
    const Scene::Frame &frame
  )
{
  for (auto &body : bodies) {
    auto &new_body_item = addItemTo(parent_item);
    buildBodyItem(new_body_item,body,frame);
  }
}


static Item makeItems(const Scene *scene_ptr)
{
  Item root;

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


static bool
  makePath(
    vector<int> &path,
    const Scene::Bodies &bodies,
    int depth,
    const Scene::Body &body
  )
{
  int n_bodies = bodies.size();

  for (int i=0; i!=n_bodies; ++i) {
    path.push_back(i);

    if (&bodies[i]==&body) {
      return true;
    }

    if (makePath(path,bodies[i].children,depth+1,body)) {
      return true;
    }

    path.pop_back();
  }

  return false;
}


void SceneTree::notifyBodyAdded(const Scene::Body &body)
{
  vector<int> path;
  assert(scene_ptr);
  Scene &scene = *scene_ptr;

  if (!makePath(path,scene.bodies(),0,body)) {
    // Couldn't find the body that was added in the scene.
    assert(false);
  }

  Item new_body_item;
  buildBodyItem(new_body_item,body,scene.displayFrame());
  insertItem(path,new_body_item);
  // Need to get the item for the parent body, find the position where
  // the new body goes and insert it.
}
