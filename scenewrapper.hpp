#include "scene.hpp"
#include "wrapper.hpp"


struct SceneWrapper : VoidWrapper {
  using NotifyFunction =
    std::function<void(const Wrapper::OperationHandler &)>;

  Scene &scene;
  const NotifyFunction notify;
  Label label_member;

  SceneWrapper(Scene &scene_arg,NotifyFunction notify,const std::string &label);

  std::vector<OperationName> operationNames() const;

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      OperationHandler &handler
    ) const override;

  void withChildWrapper(int child_index,const WrapperVisitor &) const;
  Label label() const override;
  int nChildren() const override { return scene.nBodies(); }
};
