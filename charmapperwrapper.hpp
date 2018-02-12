#include "charmapper.hpp"
#include "wrapper.hpp"


struct CharmapperWrapper : VoidWrapper {
  Charmapper &charmapper;

  CharmapperWrapper(Charmapper &charmapper_arg)
  : charmapper(charmapper_arg)
  {
  }

  void
    withOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const override;

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

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
