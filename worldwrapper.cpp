#include "worldwrapper.hpp"

#include <iostream>
#include <sstream>
#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"

using std::cerr;
using std::vector;
using std::string;
using std::ostringstream;


static void
  forEachBody(
    const vector<Scene::Body> &bodies,
    const std::function<void(const Scene::Body &)> &f
  )
{
  int n_bodies = bodies.size();

  for (int i=0; i!=n_bodies; ++i) {
    f(bodies[i]);
    forEachBody(bodies[i].children,f);
  }
}

static void
  forEachBody(
    const Scene &scene,
    const std::function<void(const Scene::Body &)> &f
  )
{
  forEachBody(scene.bodies(),f);
}

namespace {
struct WorldSceneList : CharmapperWrapper::SceneList {
  const World &world;

  WorldSceneList(const World &world_arg)
  : world(world_arg)
  {
  }

  std::vector<std::string> allBodyNames() const override
  {
    struct Visitor : World::ConstMemberVisitor {
      int &scene_index;
      vector<string> &body_names;

      Visitor(int &scene_index_arg,vector<string> &body_names_arg)
      : scene_index(scene_index_arg),
        body_names(body_names_arg)
      {
      }

      virtual void visitCharmapper(const World::CharmapperMember &) const
      {
      }

      virtual void visitScene(const World::SceneMember &scene_member) const
      {
        ++scene_index;

        forEachBody(scene_member.scene,[this](const Scene::Body &body){
          ostringstream stream;
          stream << "scene " << scene_index << ":" << body.name;
          body_names.push_back(stream.str());
        });
      }
    };

    int scene_index = 0;
    vector<string> body_names;
    Visitor visitor(scene_index,body_names);
    int n_members = world.nMembers();

    for (int i=0; i!=n_members; ++i) {
      world.visitMember(i,visitor);
    }

    return body_names;
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
	CharmapperWrapper(
	  charmapper_member.charmapper,
	  scene_list
	).handleSceneChange(operation_handler,{member_index});
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
    visitor(CharmapperWrapper{member.charmapper,WorldSceneList(world)});
  }

  virtual void visitScene(World::SceneMember &member) const
  {
    auto notify =
      [&,&world=world]
      (const Wrapper::OperationHandler &operation_handler)
      {
	if (member.scene_viewer_ptr) {
	  member.scene_viewer_ptr->notifySceneChanged();
	}

	// Update the body comboboxes in the charmappers.
	// This doesn't work because the wrappers don't necessarily exist
	// when the function is executed.

	notifyCharmappersOfSceneChange(world,operation_handler);
      };

    visitor(SceneWrapper{member.scene,notify});
  }
};
}


std::vector<std::string> WorldWrapper::operationNames() const
{
  return {"Add Charmapper","Add Scene"};
}


void
  WorldWrapper::withOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
  visitor(
    operationNames()[0],
    [path,this](TreeOperationHandler &handler){
      int index = world.nMembers();
      world.addCharmapper();
      handler.addItem(join(path,index));
    }
  );
  visitor(
    operationNames()[1],
    [path,this](TreeOperationHandler &handler){
      int index = world.nMembers();
      world.addScene();
      handler.addItem(join(path,index));
    }
  );
}


void
  WorldWrapper::withChildWrapper(
    int child_index,const WrapperVisitor &visitor
  ) const
{
  ChildWrapperVisitor wrapper_visitor(world,visitor);

  world.visitMember(child_index,wrapper_visitor);
}
