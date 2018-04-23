#include "world.hpp"

#include <iostream>
#include "worldwrapper.hpp"
#include "generatename.hpp"

using std::make_unique;
using std::unique_ptr;
using std::cerr;
using std::function;
using std::string;
using Member = World::Member;
using CharmapperMember = World::CharmapperMember;
using SceneMember = World::SceneMember;



World::World() = default;

World::~World()
{
}


const Member* World::findMember(const string &name) const
{
  for (auto &member_ptr : world_members) {
    assert(member_ptr);
    if (member_ptr->name==name) {
      return member_ptr.get();
    }
  }

  return nullptr;
}


string World::generateMemberName(const string &prefix) const
{
  auto name_exists_function = [this](const string &name){
    return findMember(name)!=nullptr;
  };
  return generateName(prefix,name_exists_function);
}


Charmapper& World::addCharmapper()
{
  auto charmapper_member_ptr = make_unique<CharmapperMember>();
  charmapper_member_ptr->name = generateMemberName("Charmapper");
  Charmapper &charmapper = charmapper_member_ptr->charmapper;
  world_members.push_back(std::move(charmapper_member_ptr));
  return charmapper;
}


Scene& World::addScene()
{
  unique_ptr<SceneMember> scene_member_ptr = make_unique<SceneMember>();
  scene_member_ptr->name = generateMemberName("Scene");
  Scene& scene = scene_member_ptr->scene;
  SceneWindow &scene_window = createSceneViewerWindow(*scene_member_ptr);
  scene_window.setScenePtr(&scene,scene_member_ptr->name);
  scene_member_ptr->scene_window_ptr = &scene_window;
  world_members.push_back(std::move(scene_member_ptr));
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


void
  World::forEachSceneMember(
    function<void(const SceneMember &scene)> function_to_call
  ) const
{
  forEachMemberOfType<SceneMember>(function_to_call);
}


void
  World::forEachCharmapperMember(
    function<void(const World::CharmapperMember &scene)> function_to_call
  ) const
{
  forEachMemberOfType<CharmapperMember>(function_to_call);
}


void
  World::forEachCharmapperMember(
    function<void(World::CharmapperMember &scene)> function_to_call
  )
{
  forEachMemberOfType<CharmapperMember>(function_to_call);
}


void
  World::forEachSceneMember(
    function<void(World::SceneMember &scene)> function_to_call
  )
{
  forEachMemberOfType<SceneMember>(function_to_call);
}


void World::applyCharmaps()
{
  forEachSceneMember([&](SceneMember &scene_member){
    scene_member.scene.displayFrame() = scene_member.scene.backgroundFrame();
  });

  forEachCharmapperMember(
    [&](CharmapperMember &charmapper_member){
      charmapper_member.charmapper.apply();
    }
  );

  forEachSceneMember([&](const SceneMember &scene_member){
    if (scene_member.scene_window_ptr) {
      scene_member.scene_window_ptr->notifySceneChanged();
    }
  });
}


CharmapperMember &World::charmapperMember(int index)
{
  Member *member_ptr = world_members[index].get();
  assert(member_ptr);
  CharmapperMember *charmapper_member_ptr =
    dynamic_cast<CharmapperMember*>(member_ptr);
  assert(charmapper_member_ptr);
  return *charmapper_member_ptr;
}


SceneMember &World::sceneMember(int index)
{
  Member *member_ptr = world_members[index].get();
  assert(member_ptr);
  SceneMember *scene_member_ptr =
    dynamic_cast<SceneMember*>(member_ptr);
  assert(scene_member_ptr);
  return *scene_member_ptr;
}


void World::removeMember(int index)
{
  world_members.erase(world_members.begin() + index);
}
