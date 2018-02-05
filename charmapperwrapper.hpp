#include "charmapper.hpp"
#include "tree.hpp"


struct CharmapperWrapper : SimpleWrapper {
  Charmapper &charmapper;

  CharmapperWrapper(Charmapper &charmapper_arg)
  : charmapper(charmapper_arg)
  {
  }

  void
    visitOperations(
      const TreePath &path,
      const TreeItem::OperationVisitor &visitor
    ) const;

  void
    visitChildWrapper(
      const TreePath &path,
      int depth,
      const WrapperVisitor &visitor
    ) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

  virtual void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    std::cerr << "CharmapperWrapper::visitType\n";
    visitor.voidItem("Charmapper");
  }

  virtual int nChildren() const
  {
    return charmapper.passes.size();
  }
};
