#ifndef WORLDWRAPPER_HPP_
#define WORLDWRAPPER_HPP_

#include "tree.hpp"
#include "world.hpp"

struct WorldWrapper : WorldInterface {
  World &world;

  WorldWrapper(World &world_arg) : world(world_arg) { }

  virtual bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    );
};

#endif /* WORLDWRAPPER_HPP_ */
