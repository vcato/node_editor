#include "world.hpp"

#include <iostream>
#include "worldwrapper.hpp"

using std::make_unique;
using std::unique_ptr;
using std::cerr;



World::World() = default;

World::~World()
{
}


void World::addCharmapper()
{
  world_members.push_back(make_unique<CharmapperMember>());
}


void World::addScene()
{
  unique_ptr<SceneMember> scene_object_ptr = make_unique<SceneMember>();
  SceneViewer &scene_viewer = createSceneViewerWindow(*scene_object_ptr);
  scene_object_ptr->scene_viewer_ptr = &scene_viewer;
  world_members.push_back(std::move(scene_object_ptr));
}


void World::visitMember(int child_index,MemberVisitor &visitor)
{
  assert(world_members[child_index]);
  World::Member &child = *world_members[child_index];
  child.accept(visitor);
}
