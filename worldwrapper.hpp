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

  virtual void
    visitWrapper(
      const TreePath &,
      int depth,
      const std::function<void(const Wrapper &)> &
    );
};

#endif /* WORLDWRAPPER_HPP_ */
