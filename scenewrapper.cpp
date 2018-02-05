#include "scenewrapper.hpp"

using OperationVisitor = TreeItem::OperationVisitor;


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
      const TreeItem::OperationVisitor &
    ) const
  {
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int /*child_index*/,const WrapperVisitor &) const
  {
    assert(false);
  }

  virtual void visitType(const TreeItem::TypeVisitor &visitor) const
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
      scene.addBody();
      handler.addItem(path);
    }
  );
}


void
  SceneWrapper::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  visitor(BodyWrapper{scene.bodies[child_index]});
}
