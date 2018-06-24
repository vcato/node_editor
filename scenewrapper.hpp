#include "scene.hpp"
#include "wrapper.hpp"
#include "wrapperstate.hpp"


struct SceneWrapper : VoidWrapper {
  struct SceneObserver {
    using ChangedFunc = std::function<void(const Wrapper::TreeObserver &)>;
    using BodyAddedFunc =
      std::function<void(const Scene::Body&,const Wrapper::TreeObserver &)>;
    using RemovingBodyFunc =
      std::function<void(const Scene::Body&)>;
    using RemovedBodyFunc =
      std::function<void(const Wrapper::TreeObserver &)>;

    ChangedFunc changed_func;
    BodyAddedFunc body_added_func;
    RemovingBodyFunc removing_body_func;
    RemovedBodyFunc removed_body_func;

    SceneObserver(ChangedFunc func_arg) : changed_func(std::move(func_arg)) { }
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
      TreeObserver &
    ) const override;

  void setState(const WrapperState &);
  void withChildWrapper(int child_index,const WrapperVisitor &) const;
  Label label() const override;
  int nChildren() const override { return scene.nBodies(); }
};
