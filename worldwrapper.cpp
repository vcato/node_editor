#include "worldwrapper.hpp"

#include <iostream>
#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"

using std::cerr;


static void
  notifyCharmappersOfSceneChange(
    World &world,
    const Wrapper::OperationHandler &operation_handler
  )
{
  int n_members = world.nMembers();

  for (int i=0; i!=n_members; ++i) {
    struct Visitor : World::MemberVisitor {
      const int member_index;
      const Wrapper::OperationHandler &operation_handler;

      Visitor(
        int member_index_arg,
        const Wrapper::OperationHandler &operation_handler_arg
      )
      : member_index(member_index_arg),
        operation_handler(operation_handler_arg)
      {
      }

      virtual void
        visitCharmapper(World::CharmapperMember &charmapper_member)
      {
	CharmapperWrapper(
	  charmapper_member.charmapper
	).handleSceneChange(operation_handler,{member_index});
      }

      virtual void visitScene(World::SceneMember &)
      {
      }
    };

    Visitor visitor(i,operation_handler);

    world.visitMember(i,visitor);
  }
}


namespace {
struct ChildWrapperVisitor : World::MemberVisitor {
  const WorldWrapper &world_wrapper;
  const WrapperVisitor &visitor;

  ChildWrapperVisitor(
    const WorldWrapper &world_wrapper_arg,
    const std::function<void(const Wrapper&)> &visitor_arg
  )
  : world_wrapper(world_wrapper_arg),
    visitor(visitor_arg)
  {
  }

  virtual void visitCharmapper(World::CharmapperMember &member)
  {
    visitor(CharmapperWrapper{member.charmapper});
  }

  virtual void visitScene(World::SceneMember &member)
  {
    auto notify =
      [&,&world=world_wrapper.world]
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


void
  WorldWrapper::withOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
  visitor(
    "Add Charmapper",
    [path,this](TreeOperationHandler &handler){
      int index = world.nMembers();
      world.addCharmapper();
      handler.addItem(join(path,index));
    }
  );
  visitor(
    "Add Scene",
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
  ChildWrapperVisitor wrapper_visitor(*this,visitor);

  world.visitMember(child_index,wrapper_visitor);
}
