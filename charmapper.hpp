#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>


struct Charmapper {
  struct MotionPass {
  };

  std::vector<MotionPass> passes;

  void addMotionPass()
  {
    passes.push_back(MotionPass());
  }
};

#endif /* CHARMAPPER_HPP_ */
