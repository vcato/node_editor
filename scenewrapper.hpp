#include "scene.hpp"
#include "tree.hpp"


struct SceneWrapper : SimpleWrapper {
  Scene &scene;

  SceneWrapper(Scene &scene_arg)
  : scene(scene_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &path,
      const TreeItem::OperationVisitor &visitor
    ) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

  void
    visitChildWrapper(
      const TreePath &path,
      int depth,
      const WrapperVisitor &visitor
    ) const;

  virtual void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Scene");
  }
};
