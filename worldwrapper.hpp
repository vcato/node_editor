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

  virtual void
    visitWrapper(
      const TreePath &,
      int depth,
      const WrapperVisitor &
    ) const;

  void
    visitChildWrapper(
      const TreePath &path,
      int depth,
      const std::function<void(const Wrapper &)> &visitor
    ) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

  virtual void visitType(const TreeItem::TypeVisitor &) const
  {
    assert(false);
  }
};

#endif /* WORLDWRAPPER_HPP_ */
