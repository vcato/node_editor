#include "worldwrapper.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"
#include "stringutil.hpp"

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
      const string &scene_name,
      const vector<string> &body_names
    ) const
  {
    for (auto &body_name : body_names) {
      ostringstream stream;
      stream << scene_name << ":" << body_name;
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
      const string &scene_name = scene_member.name;
      addBodyNamesTo(all_body_names,scene_name,scene_body_names);
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


namespace {
struct NotifyCharmapperVisitor : World::MemberVisitor {
  using SceneList = CharmapperWrapper::SceneList;
  const int member_index;
  const Wrapper::TreeObserver &tree_observer;
  const SceneList &scene_list;

  NotifyCharmapperVisitor(
    int member_index_arg,
    const Wrapper::TreeObserver &operation_handler_arg,
    const SceneList &scene_list_arg
  )
  : member_index(member_index_arg),
    tree_observer(operation_handler_arg),
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

      virtual void removeCharmapper() const
      {
        assert(false);
      }
    };

    Callbacks callbacks{scene_list};

    CharmapperWrapper(
      charmapper_member.charmapper,
      callbacks,
      charmapper_member.name
    ).handleSceneChange(tree_observer,{member_index});

    // This may not be sufficient.  It would probably be better to
    // call world.applyCharmaps().  Need an example of where this doesn't
    // work.
    charmapper_member.charmapper.apply();
  }

  virtual void visitScene(World::SceneMember &) const
  {
  }
};

}


static void
  notifyCharmappersOfSceneChange(
    World &world,
    const Wrapper::TreeObserver &tree_observer
  )
{
  int n_members = world.nMembers();
  WorldSceneList scene_list(world);

  for (int member_index=0; member_index!=n_members; ++member_index) {
    NotifyCharmapperVisitor visitor(member_index,tree_observer,scene_list);
    world.visitMember(member_index,visitor);
  }
}


namespace {
struct ChildWrapperVisitor : World::MemberVisitor {
  World &world;
  const WrapperVisitor &visitor;
  const int member_index;

  ChildWrapperVisitor(
    World &world_arg,
    const std::function<void(const Wrapper&)> &visitor_arg,
    int member_index_arg
  )
  : world(world_arg),
    visitor(visitor_arg),
    member_index(member_index_arg)
  {
  }

  virtual void visitCharmapper(World::CharmapperMember &member) const
  {
    WorldSceneList scene_list(world);
    struct Callbacks : CharmapperWrapper::Callbacks {
      Charmapper &charmapper;
      World &world;
      const int member_index;

      Callbacks(
        const CharmapperWrapper::SceneList &scene_list,
        Charmapper &charmapper_arg,
        World &world_arg,
        int member_index_arg
      )
      : CharmapperWrapper::Callbacks(scene_list),
        charmapper(charmapper_arg),
        world(world_arg),
        member_index(member_index_arg)
      {
      }

      virtual void notifyCharmapChanged() const
      {
        world.applyCharmaps();
      }

      virtual void removeCharmapper() const
      {
        world.removeMember(member_index);
      }
    };

    Callbacks callbacks{scene_list,member.charmapper,world,member_index};

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
    auto changed_func =
      [&,&world=world]
      (const Wrapper::TreeObserver &tree_observer)
      {
	// Update the body comboboxes in the charmappers.
	// This doesn't work because the wrappers don't necessarily exist
	// when the function is executed.

	member.scene.displayFrame() = member.scene.backgroundFrame();

	notifyCharmappersOfSceneChange(world,tree_observer);

	// Notify the scene window after notifying charmapper
	if (member.scene_window_ptr) {
	  member.scene_window_ptr->notifySceneChanged();
	}
      };

    // We also need to get an tree observer
    auto body_added_func =
      [&](
        const Scene::Body &body,
        const Wrapper::TreeObserver &tree_observer
      )
      {
        notifyCharmappersOfSceneChange(world,tree_observer);

        if (member.scene_window_ptr) {
          member.scene_window_ptr->notifyBodyAdded(body);
        }
      };

    auto removing_body_func = [&](const Scene::Body &body)
      {
        if (member.scene_window_ptr) {
          member.scene_window_ptr->notifyRemovingBody(body);
        }
      };

    auto removed_body_func =
      [&](const Wrapper::TreeObserver &tree_observer)
      {
        notifyCharmappersOfSceneChange(world,tree_observer);

        if (member.scene_window_ptr) {
          member.scene_window_ptr->notifySceneChanged();
        }
      };

    SceneWrapper::SceneObserver callbacks(changed_func);
    callbacks.body_added_func = body_added_func;
    callbacks.removing_body_func = removing_body_func;
    callbacks.removed_body_func = removed_body_func;
    visitor(SceneWrapper{member.scene,callbacks,member.name});
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
    TreeObserver &tree_observer
  ) const
{
  switch (operation_index) {
    case 0:
      {
        int index = world.nMembers();
        world.addCharmapper();
        tree_observer.itemAdded(join(path,index));
      }
      return;
    case 1:
      {
        int index = world.nMembers();
        world.addScene();
        tree_observer.itemAdded(join(path,index));
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
  int member_index = child_index;
  ChildWrapperVisitor wrapper_visitor(world,visitor,member_index);

  world.visitMember(member_index,wrapper_visitor);
}


void WorldWrapper::setState(const WrapperState &state) const
{
  if (world.nMembers()==0 && state.children.empty()) {
    return;
  }

  if (world.nMembers()!=0) {
    assert(false);
  }

  for (const WrapperState &child_state : state.children) {
    const string &tag = child_state.tag;
    if (startsWith(tag,"scene")) {
      Scene &scene = world.addScene(); // we need to pass the label here
      if (!child_state.children.empty()) {
        auto changed_func = [](const Wrapper::TreeObserver &){};
        SceneWrapper::SceneObserver callbacks(changed_func);
        string label;

        if (tag=="scene1") {
          label = "Scene1";
        }

        SceneWrapper(scene,callbacks,label).setState(child_state);
      }
    }
    else {
      if (startsWith(child_state.tag,"charmapper")) {
        assert(false);
      }
      cerr << "child_state.tag: " << child_state.tag << "\n";
      assert(false);
    }
  }
}
