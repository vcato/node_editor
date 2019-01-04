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


static void
  notifyCharmappersOfSceneChange(
    World &world,
    const Wrapper::TreeObserver &tree_observer
  )
{
  function<void(World::CharmapperMember &,int)> handle_scene_change_function =
    [&](World::CharmapperMember &charmapper_member,int member_index)
  {
    WorldSceneList scene_list(world);

    CharmapperWrapper::handleSceneChange(
      charmapper_member.charmapper,
      charmapper_member.name,
      tree_observer,
      /*charmapper_path*/{member_index},
      world.observed_diagrams,
      scene_list
    );

    world.applyCharmaps();
  };

  world.forEachCharmapperMember(handle_scene_change_function);
}


namespace {
struct ChildWrapperVisitor : World::MemberVisitor {
  World &world;
  const WrapperVisitor &visitor;
  const int member_index;

  std::function<void()> &charmap_changed_function;

  ChildWrapperVisitor(
    World &world_arg,
    const std::function<void(const Wrapper&)> &visitor_arg,
    int member_index_arg,
    std::function<void()> &charmap_changed_function_arg
  )
  : world(world_arg),
    visitor(visitor_arg),
    member_index(member_index_arg),
    charmap_changed_function(charmap_changed_function_arg)
  {
  }

  virtual void visitCharmapper(World::CharmapperMember &member) const
  {
    struct Callbacks : CharmapperWrapper::Callbacks {
      std::function<void()> &charmap_changed_function;
      World &world;
      const int member_index;

      Callbacks(
        std::function<void()> &charmap_changed_function_arg,
        World &world_arg,
        int member_index_arg
      )
      : charmap_changed_function(charmap_changed_function_arg),
        world(world_arg),
        member_index(member_index_arg)
      {
      }

      void notifyCharmapChanged() const override
      {
        charmap_changed_function();
      }

      void removeCharmapper() const override
      {
        world.removeMember(member_index);
      }
    };

    WorldSceneList scene_list(world);
    Callbacks callbacks{
      charmap_changed_function,
      world,
      member_index
    };
    bool diagram_observer_created = false;

    CharmapperWrapper::WrapperData
      wrapper_data{
        scene_list,
        world.observed_diagrams,
        callbacks,
        diagram_observer_created
      };

    visitor(
      CharmapperWrapper{
        member.charmapper,
        wrapper_data,
        member.name
      }
    );

    if (diagram_observer_created) {
      // If we created a diagram observer, then we need to reapply the
      // charmaps to update the observed diagram states.
      world.applyCharmaps();
    }
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
    visitor(SceneWrapper{member.scene,&callbacks,member.name});
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
  function<void()> charmap_changed_function =
    [&world = this->world](){
      world.applyCharmaps();
    };
  ChildWrapperVisitor
    wrapper_visitor(
      world,
      visitor,
      member_index,
      charmap_changed_function
    );

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

  int child_index = 0;

  for (const WrapperState &child_state : state.children) {
    const string &tag = child_state.tag;

    if (startsWith(tag,"scene")) {
      world.addScene(); // we need to pass the label here

      withChildWrapper(child_index,[&](const Wrapper &child_wrapper){
        child_wrapper.setState(child_state);
      });
    }
    else if (startsWith(tag,"charmapper")) {
      world.addCharmapper(); // we need to pass the label here

      auto visitor =
        [&](const Wrapper &child_wrapper){
          child_wrapper.setState(child_state);
        };
      int member_index = child_index;
      // Defer applying charmaps as they are changing, since the time
      // will be wasted on these intermediary changes.
      function<void()> charmap_changed_function = [](){};
      ChildWrapperVisitor
        wrapper_visitor(
          world,
          visitor,
          member_index,
          charmap_changed_function
        );

      world.visitMember(member_index,wrapper_visitor);
    }
    else {
      cerr << "child_state.tag: " << child_state.tag << "\n";
      assert(false);
    }

    ++child_index;
  }

  world.applyCharmaps();
}
