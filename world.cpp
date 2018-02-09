#include "world.hpp"

#include <iostream>
#include "worldwrapper.hpp"

using std::make_unique;
using std::unique_ptr;
using std::cerr;


struct World::WorldMember {
  using MemberVisitor = World::MemberVisitor;

  virtual void accept(MemberVisitor &) = 0;
  virtual ~WorldMember() {}

  struct CharmapperObject;
  struct SceneObject;
};


struct World::WorldMember::CharmapperObject : World::WorldMember {
  Charmapper charmapper;

  virtual void accept(MemberVisitor &visitor)
  {
    visitor.visitCharmapper(charmapper);
  }
};


struct World::WorldMember::SceneObject : World::WorldMember {
  Scene scene;

  virtual void accept(MemberVisitor &visitor)
  {
    visitor.visitScene(scene);
  }
};


World::World() = default;

World::~World()
{
}


void World::addCharmapper()
{
  using CharmapperObject = WorldMember::CharmapperObject;
  world_members.push_back(make_unique<CharmapperObject>());
}


void World::addScene()
{
  using SceneObject = WorldMember::SceneObject;
  unique_ptr<SceneObject> scene_object_ptr = make_unique<SceneObject>();
  Scene &scene = scene_object_ptr->scene;
  world_members.push_back(std::move(scene_object_ptr));
  createSceneWindow(scene);
}


void World::visitMember(int child_index,MemberVisitor &visitor)
{
  assert(world_members[child_index]);
  World::WorldMember &child = *world_members[child_index];
  child.accept(visitor);
}
