#include "charmapper.hpp"
#include "wrapper.hpp"


struct CharmapperWrapper : SimpleWrapper {
  Charmapper &charmapper;

  CharmapperWrapper(Charmapper &charmapper_arg)
  : charmapper(charmapper_arg)
  {
  }

  void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const;

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.voidItem();
  }

  virtual std::string label() const
  {
    return "Charmapper";
  }

  virtual int nChildren() const
  {
    return charmapper.passes.size();
  }
};
