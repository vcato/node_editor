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


Charmapper& World::addCharmapper()
{
  auto charmapper_member_ptr = make_unique<CharmapperMember>();
  Charmapper &charmapper = charmapper_member_ptr->charmapper;
  world_members.push_back(std::move(charmapper_member_ptr));
  return charmapper;
}


Scene& World::addScene()
{
  unique_ptr<SceneMember> scene_object_ptr = make_unique<SceneMember>();
  Scene& scene = scene_object_ptr->scene;
  SceneViewer &scene_viewer = createSceneViewerWindow(*scene_object_ptr);
  scene_object_ptr->scene_viewer_ptr = &scene_viewer;
  world_members.push_back(std::move(scene_object_ptr));
  return scene;
}


void World::visitMember(int child_index,const MemberVisitor &visitor)
{
  assert(world_members[child_index]);
  World::Member &child = *world_members[child_index];
  child.accept(visitor);
}


void
  World::visitMember(int child_index, const ConstMemberVisitor &visitor) const
{
  assert(world_members[child_index]);
  const World::Member &child = *world_members[child_index];
  child.acceptConst(visitor);
}
