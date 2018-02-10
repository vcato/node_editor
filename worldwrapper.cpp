#include "worldwrapper.hpp"

#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"


namespace {
struct ChildWrapperVisitor : World::MemberVisitor {
  const WrapperVisitor &visitor;

  ChildWrapperVisitor(
    const std::function<void(const Wrapper&)> &visitor_arg
  )
  : visitor(visitor_arg)
  {
  }

  virtual void visitCharmapper(World::CharmapperMember &member)
  {
    visitor(CharmapperWrapper{member.charmapper});
  }

  virtual void visitScene(World::SceneMember &member)
  {
    auto notify = [&](){
      if (member.scene_viewer_ptr) {
        member.scene_viewer_ptr->notifySceneChanged();
      }
    };

    visitor(SceneWrapper{member.scene,notify});
  }
};
}


void
  WorldWrapper::visitOperations(
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
  ChildWrapperVisitor wrapper_visitor(visitor);

  world.visitMember(child_index,wrapper_visitor);
}
