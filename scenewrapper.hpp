#include "scene.hpp"
#include "wrapper.hpp"
#include "wrapperstate.hpp"


class SceneWrapper : public VoidWrapper {
  public:
    struct SceneObserver;

    SceneWrapper(
      Scene &scene_arg,
      const SceneObserver* notify_ptr,
      const std::string &label
    );

    std::vector<OperationName> operationNames() const override;

    void makeVariablePath(TreePath &,int frame_index,int variable_index) const;

    void
      executeOperation(
        int operation_index,
        const TreePath &path,
        TreeObserver &
      ) const override;

    void setState(const WrapperState &) const override;
    void
      withChildWrapper(int child_index,const WrapperVisitor &) const override;
    Label label() const override;
    int nChildren() const override;

    static int currentFrameChildIndex() { return 1; }
    static int firstBodyChildIndex() { return 2; }

    struct SceneObserver {
      using ChangedFunc = std::function<void()>;
      using BodyAddedFunc =
        std::function<void(const Scene::Body&,Wrapper::TreeObserver &)>;
      using RemovedBodyFunc =
        std::function<void(Wrapper::TreeObserver &)>;
      using RemoveFunc = std::function<void(Wrapper::TreeObserver &)>;
      using RemovingBodyFunc =
        std::function<void(const Scene::Body&)>;

      ChangedFunc changed_func;
      BodyAddedFunc body_added_func;
      RemovingBodyFunc removing_body_func;
      RemovedBodyFunc removed_body_func;
      RemoveFunc remove_func;

      SceneObserver(ChangedFunc func_arg)
      : changed_func(std::move(func_arg))
      {
      }
    };

  private:
    struct Impl;

    Scene &scene;
    const SceneObserver &callbacks;
    Label label_member;
};
