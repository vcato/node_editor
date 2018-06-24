#ifndef WORLDWRAPPER_HPP_
#define WORLDWRAPPER_HPP_

#include "wrapper.hpp"
#include "world.hpp"

struct WorldWrapper : VoidWrapper
{
  World &world;

  WorldWrapper(World &world_arg) : world(world_arg) { }

  std::vector<std::string> operationNames() const;

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &
    ) const override;

  void
    withChildWrapper(
      int child_index,const WrapperVisitor &visitor
    ) const;

  virtual Label label() const
  {
    return "world";
  }

  virtual int nChildren() const
  {
    return world.nMembers();
  }
};

#endif /* WORLDWRAPPER_HPP_ */
