#include "scenewrapper.hpp"

namespace {
struct BodyWrapper : SimpleWrapper {
  Scene::Body &body;

  BodyWrapper(Scene::Body &body_arg)
  : body(body_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &,
      const OperationVisitor &
    ) const
  {
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int /*child_index*/,const WrapperVisitor &) const
  {
    assert(false);
  }

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.voidItem("Body");
  }

  virtual int nChildren() const
  {
    return 0;
  }
};
}


void
  SceneWrapper::visitOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
  Scene &scene = this->scene;
  visitor(
    "Add Body",
    [path,&scene](TreeOperationHandler &handler){
      int index = scene.nBodies();
      scene.addBody();
      handler.addItem(join(path,index));
    }
  );
}


void
  SceneWrapper::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  visitor(BodyWrapper{scene.bodies()[child_index]});
}
