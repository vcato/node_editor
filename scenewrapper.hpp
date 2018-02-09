#include "scene.hpp"
#include "wrapper.hpp"


struct SceneWrapper : SimpleWrapper {
  Scene &scene;

  SceneWrapper(Scene &scene_arg)
  : scene(scene_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int child_index,const WrapperVisitor &) const;

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.voidItem("Scene");
  }

  virtual int nChildren() const
  {
    return scene.bodies.size();
  }
};
