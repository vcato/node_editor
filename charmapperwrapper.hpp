#include "charmapper.hpp"
#include "wrapper.hpp"


struct CharmapperWrapper : VoidWrapper {
  struct SceneList {
    virtual std::vector<std::string> allBodyNames() const = 0;
  };

  Charmapper &charmapper;
  const SceneList &scene_list;

  CharmapperWrapper(Charmapper &charmapper_arg,const SceneList &scene_list_arg)
  : charmapper(charmapper_arg),
    scene_list(scene_list_arg)
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
    return charmapper.passes.size();
  }

  void
    handleSceneChange(
      const OperationHandler &,
      const TreePath &charmapper_path
    );
};
