#include "scene.hpp"
#include "wrapper.hpp"


struct SceneWrapper : VoidWrapper {
  Scene &scene;
  const std::function<void()> notify;

  SceneWrapper(
    Scene &scene_arg,
    std::function<void()> notify
  );

  virtual void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const;

  virtual Diagram *diagramPtr() const { return nullptr; }
  void withChildWrapper(int child_index,const WrapperVisitor &) const;
  std::string label() const override { return "Scene"; }
  int nChildren() const override { return scene.nBodies(); }
};
