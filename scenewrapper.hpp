#include "scene.hpp"
#include "wrapper.hpp"


struct SceneWrapper : VoidWrapper {
  using NotifyFunction =
    std::function<void(const Wrapper::OperationHandler &)>;

  Scene &scene;
  const NotifyFunction notify;

  SceneWrapper(Scene &scene_arg,NotifyFunction notify);

  std::vector<OperationName> operationNames() const;

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      OperationHandler &handler
    ) const override;

  virtual Diagram *diagramPtr() const { return nullptr; }
  void withChildWrapper(int child_index,const WrapperVisitor &) const;
  std::string label() const override { return "Scene"; }
  int nChildren() const override { return scene.nBodies(); }
};
