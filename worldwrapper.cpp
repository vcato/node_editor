#include "worldwrapper.hpp"

#include "charmapperwrapper.hpp"
#include "scenewrapper.hpp"


namespace {
struct ChildWrapperVisitor2 : World::MemberVisitor {
  const WrapperVisitor &visitor;

  ChildWrapperVisitor2(
    const std::function<void(const Wrapper&)> &visitor_arg
  )
  : visitor(visitor_arg)
  {
  }

  virtual void visitCharmapper(Charmapper &charmapper)
  {
    visitor(CharmapperWrapper{charmapper});
  }

  virtual void visitScene(Scene &scene)
  {
    visitor(SceneWrapper{scene});
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
  ChildWrapperVisitor2 wrapper_visitor(visitor);

  world.visitMember(child_index,wrapper_visitor);
}
