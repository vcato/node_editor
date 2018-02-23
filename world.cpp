#include "world.hpp"

#include <iostream>
#include "worldwrapper.hpp"

using std::make_unique;
using std::unique_ptr;
using std::cerr;
using std::function;



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
  SceneWindow &scene_window = createSceneViewerWindow(*scene_object_ptr);
  scene_window.setScenePtr(&scene);
  scene_object_ptr->scene_window_ptr = &scene_window;
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


#if 0
const Scene& World::scene(int scene_number)
{
  int scene_index = 0;
  const Scene *scene_ptr = nullptr;

  forEachSceneMember([&](const SceneMember &scene_member){
    if (scene_index==scene_number) {
      scene_ptr = &scene_member.scene;
    }

    ++scene_index;
  });

  assert(scene_ptr);

  return *scene_ptr;
}
#endif


void
  World::forEachSceneMember(
    function<void(const World::SceneMember &scene)> f
  ) const
{
  struct Visitor : World::ConstMemberVisitor {
    const function<void(const World::SceneMember &)> &f;

    Visitor(const function<void(const World::SceneMember &)> &f_arg)
    : f(f_arg)
    {
    }

    virtual void visitCharmapper(const World::CharmapperMember &) const
    {
    }

    virtual void visitScene(const World::SceneMember &scene_member) const
    {
      f(scene_member);
    }
  };

  Visitor visitor(f);
  int n_members = nMembers();

  for (int i=0; i!=n_members; ++i) {
    visitMember(i,visitor);
  }
}


void
  World::forEachSceneMember(
    function<void(World::SceneMember &scene)> f
  )
{
  struct Visitor : World::MemberVisitor {
    const function<void(World::SceneMember &)> &f;

    Visitor(const function<void(World::SceneMember &)> &f_arg)
    : f(f_arg)
    {
    }

    virtual void visitCharmapper(World::CharmapperMember &) const
    {
    }

    virtual void visitScene(World::SceneMember &scene_member) const
    {
      f(scene_member);
    }
  };

  Visitor visitor(f);
  int n_members = nMembers();

  for (int i=0; i!=n_members; ++i) {
    visitMember(i,visitor);
  }
}
