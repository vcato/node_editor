#include "scene.hpp"
#include "wrapper.hpp"


struct SceneWrapper : VoidWrapper {
  struct SceneObserver {
    using ChangedFunc = std::function<void(const Wrapper::OperationHandler &)>;
    using BodyAddedFunc =
      std::function<void(const Scene::Body&,const Wrapper::OperationHandler &)>;

    ChangedFunc changed_func;
    BodyAddedFunc body_added_func;

    SceneObserver(const ChangedFunc &func_arg) : changed_func(func_arg) { }
  };

  Scene &scene;
  const SceneObserver &callbacks;
  Label label_member;

  SceneWrapper(
    Scene &scene_arg,
    const SceneObserver& notify,
    const std::string &label
  );

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
