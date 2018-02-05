#ifndef WORLDWRAPPER_HPP_
#define WORLDWRAPPER_HPP_

#include "tree.hpp"
#include "world.hpp"

struct WorldWrapper : SimpleWrapper
{
  using OperationVisitor = TreeItem::OperationVisitor;
  World &world;

  WorldWrapper(World &world_arg) : world(world_arg) { }

  virtual void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const;

  void
    withChildWrapper(
      int child_index,const WrapperVisitor &visitor
    ) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

  virtual void visitType(const TreeItem::TypeVisitor &) const
  {
    assert(false);
  }

  virtual int nChildren() const
  {
    return world.nMembers();
  }
};

#endif /* WORLDWRAPPER_HPP_ */
