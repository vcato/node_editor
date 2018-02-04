#ifndef WORLDWRAPPER_HPP_
#define WORLDWRAPPER_HPP_

#include "tree.hpp"
#include "world.hpp"

struct WorldWrapper : WorldInterface, Wrapper {
  World &world;

  WorldWrapper(World &world_arg) : world(world_arg) { }

  virtual bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    );

  virtual void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const;

#if 1
  virtual void
    visitWrapper(
      const TreePath &,
      int depth,
      const WrapperVisitor &
    ) const;
#endif

  void
    visitChildWrapper(
      const TreePath &path,
      int depth,
      const std::function<void(const Wrapper &)> &visitor
    ) const;

  // virtual void visitChild(int child_index,const WrapperVisitor &);

  virtual Diagram *diagramPtr() const { return nullptr; }
};

#endif /* WORLDWRAPPER_HPP_ */
