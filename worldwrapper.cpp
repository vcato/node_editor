#include "worldwrapper.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"

using std::cerr;
using std::vector;
using std::map;
using std::string;
using std::ostringstream;
using std::function;
using Body = Scene::Body;


template <typename Bodies,typename Body>
static void
  forEachBody(
    Bodies &bodies,
    const std::function<void(Body &)> &f
  )
{
  int n_bodies = bodies.size();

  for (int i=0; i!=n_bodies; ++i) {
    f(bodies[i]);
    forEachBody(bodies[i].children,f);
  }
}

template <typename Scene,typename Body>
static void
  forEachBodyInScene(
    Scene &scene,
    const std::function<void(Body &)> &f
  )
{
  forEachBody(scene.bodies(),f);
}


template <typename T>
static void appendTo(vector<T> &container,const vector<T> &new_elements)
{
  container.insert(container.end(),new_elements.begin(),new_elements.end());
}


namespace {
struct WorldSceneList : CharmapperWrapper::SceneList {
  World &world;

  WorldSceneList(World &world_arg)
  : world(world_arg)
  {
  }

  std::vector<std::string> sceneBodyNames(const Scene &scene) const
  {
    vector<string> scene_body_names;

    function<void(const Scene::Body&)> add_body_name_function =
      [&](const Scene::Body &body){
        scene_body_names.push_back(body.name);
      };

    forEachBodyInScene(scene,add_body_name_function);

    return scene_body_names;
  }

  BodyLinks sceneBodyLinks(Scene &scene) const
  {
    BodyLinks result;

    function<void(Scene::Body&)> add_body_function =
      [&](Scene::Body &body){
        result.push_back(BodyLink(&scene,&body));
      };

    forEachBodyInScene(scene,add_body_function);

    return result;
  }

  void
    addBodyNamesTo(
      vector<string> &all_body_names,
      int scene_index,
      const vector<string> &body_names
    ) const
  {
    for (auto &body_name : body_names) {
      ostringstream stream;
      stream << "scene " << scene_index << ":" << body_name;
      all_body_names.push_back(stream.str());
    }
  }

  BodyLinks allBodyLinks() const override
  {
    BodyLinks all_bodies;

    world.forEachSceneMember([&](World::SceneMember &scene_member){
      appendTo(all_bodies,sceneBodyLinks(scene_member.scene));
    });

    return all_bodies;
  }

  vector<string> allBodyNames() const override
  {
    int scene_index = 0;
    vector<string> all_body_names;

    world.forEachSceneMember([&](const World::SceneMember &scene_member){
      vector<string> scene_body_names = sceneBodyNames(scene_member.scene);
      addBodyNamesTo(all_body_names,scene_index,scene_body_names);
      ++scene_index;
    });

    return all_body_names;
  }

  int nScenes() override
  {
    int n_scenes = 0;

    world.forEachSceneMember(
      [&](const World::SceneMember &){
        ++n_scenes;
      }
    );

    return n_scenes;
  }
};
}


static void
  notifyCharmappersOfSceneChange(
    World &world,
    const Wrapper::OperationHandler &operation_handler
  )
{
  int n_members = world.nMembers();
  using SceneList = CharmapperWrapper::SceneList;
  WorldSceneList scene_list(world);

  for (int i=0; i!=n_members; ++i) {
    struct Visitor : World::MemberVisitor {
      const int member_index;
      const Wrapper::OperationHandler &operation_handler;
      const SceneList &scene_list;

      Visitor(
        int member_index_arg,
        const Wrapper::OperationHandler &operation_handler_arg,
        const SceneList &scene_list_arg
      )
      : member_index(member_index_arg),
        operation_handler(operation_handler_arg),
        scene_list(scene_list_arg)
      {
      }

      virtual void
        visitCharmapper(World::CharmapperMember &charmapper_member) const
      {
        struct Callbacks : CharmapperWrapper::Callbacks {
          Callbacks(const SceneList &scene_list_arg)
          : CharmapperWrapper::Callbacks(scene_list_arg)
          {
          }

          virtual void notifyCharmapChanged() const
          {
            // The charmap changed due to a scene change.  We don't want
            // to cause an inifinite loop, so we don't notify the scene
            // of the charmap change.
          }
        };

        Callbacks callbacks{scene_list};

	CharmapperWrapper(
	  charmapper_member.charmapper,
	  callbacks,
          charmapper_member.name
	).handleSceneChange(operation_handler,{member_index});

	charmapper_member.charmapper.apply();
      }

      virtual void visitScene(World::SceneMember &) const
      {
      }
    };

    Visitor visitor(i,operation_handler,scene_list);

    world.visitMember(i,visitor);
  }
}


namespace {
struct ChildWrapperVisitor : World::MemberVisitor {
  World &world;
  const WrapperVisitor &visitor;

  ChildWrapperVisitor(
    World &world_arg,
    const std::function<void(const Wrapper&)> &visitor_arg
  )
  : world(world_arg),
    visitor(visitor_arg)
  {
  }

  virtual void visitCharmapper(World::CharmapperMember &member) const
  {
    WorldSceneList scene_list(world);
    struct Callbacks : CharmapperWrapper::Callbacks {
      Charmapper &charmapper;
      World &world;

      Callbacks(
        const CharmapperWrapper::SceneList &scene_list,
        Charmapper &charmapper_arg,
        World &world_arg
      )
      : CharmapperWrapper::Callbacks(scene_list),
        charmapper(charmapper_arg),
        world(world_arg)
      {
      }

      virtual void notifyCharmapChanged() const
      {
        world.applyCharmaps();
      }
    };

    Callbacks callbacks{scene_list,member.charmapper,world};
    visitor(
      CharmapperWrapper{
        member.charmapper,
        callbacks,
        member.name
      }
    );
  }

  virtual void visitScene(World::SceneMember &member) const
  {
    auto notify =
      [&,&world=world]
      (const Wrapper::OperationHandler &operation_handler)
      {
	// Update the body comboboxes in the charmappers.
	// This doesn't work because the wrappers don't necessarily exist
	// when the function is executed.

	member.scene.displayFrame() = member.scene.backgroundFrame();

	notifyCharmappersOfSceneChange(world,operation_handler);

	// Notify the scene window after notifying charmapper
	if (member.scene_window_ptr) {
	  member.scene_window_ptr->notifySceneChanged();
	}
      };

    visitor(SceneWrapper{member.scene,notify,member.name});
  }
};
}


std::vector<std::string> WorldWrapper::operationNames() const
{
  return {"Add Charmapper","Add Scene"};
}


void
  WorldWrapper::executeOperation(
    int operation_index,
    const TreePath &path,
    OperationHandler &handler
  ) const
{
  switch (operation_index) {
    case 0:
      {
        int index = world.nMembers();
        world.addCharmapper();
        handler.addItem(join(path,index));
      }
      return;
    case 1:
      {
        int index = world.nMembers();
        world.addScene();
        handler.addItem(join(path,index));
      }
      return;
  }

  assert(false);
}


void
  WorldWrapper::withChildWrapper(
    int child_index,const WrapperVisitor &visitor
  ) const
{
  ChildWrapperVisitor wrapper_visitor(world,visitor);

  world.visitMember(child_index,wrapper_visitor);
}
