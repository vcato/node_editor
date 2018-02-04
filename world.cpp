#include "world.hpp"

#include "worldpolicies.hpp"
#include "worldwrapper.hpp"

using std::make_unique;


struct World::WorldMember {
  using MemberVisitor = World::MemberVisitor;

  virtual void accept(MemberVisitor &) = 0;
};


namespace {
struct CharmapperObject : World::WorldMember {
  Charmapper charmapper;

  virtual void accept(MemberVisitor &visitor)
  {
    visitor.visitCharmapper(charmapper);
  }
};
}


namespace {
struct SceneObject : World::WorldMember {
  Scene scene;

  virtual void accept(MemberVisitor &visitor)
  {
    visitor.visitScene(scene);
  }
};
}


World::World() = default;
World::~World() = default;


void World::addCharmapper()
{
  world_members.push_back(make_unique<CharmapperObject>());
}


void World::addScene()
{
  world_members.push_back(make_unique<SceneObject>());
  createSceneWindow();
}


void World::visitMember(int child_index,MemberVisitor &visitor)
{
  assert(world_members[child_index]);
  World::WorldMember &child = *world_members[child_index];
  child.accept(visitor);
}
