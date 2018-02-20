#include "charmapper.hpp"
#include "wrapper.hpp"


struct CharmapperWrapper : VoidWrapper {
  struct SceneList {
    // using State = std::vector<Scene>;
    // State state() const;
    // void setState(const State &);
    virtual int nScenes() = 0;
    virtual std::vector<std::string> allBodyNames() const = 0;
    virtual std::vector<Scene::Body *> allBodyPtrs() const = 0;
  };

  struct Callbacks {
    const SceneList &scene_list;
    virtual void notifyCharmapChanged() const = 0;

    Callbacks(const SceneList &scene_list_arg)
    : scene_list(scene_list_arg)
    {
    }
  };

  Charmapper &charmapper;
  const Callbacks &callbacks;

  CharmapperWrapper(
    Charmapper &charmapper_arg,
    const Callbacks &callbacks_arg
  )
  : charmapper(charmapper_arg),
    callbacks(callbacks_arg)
  {
  }

  std::vector<std::string> operationNames() const;

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      OperationHandler &handler
    ) const override;

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const;

  virtual std::string label() const
  {
    return "Charmapper";
  }

  virtual int nChildren() const
  {
    return charmapper.nPasses();
  }

  void
    handleSceneChange(
      const OperationHandler &,
      const TreePath &charmapper_path
    );
};
