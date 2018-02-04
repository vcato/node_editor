#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>
#include <memory>


struct Charmapper {
  Charmapper() = default;
  Charmapper(const Charmapper &) = delete;

  struct MotionPass {
    MotionPass();
    MotionPass(const MotionPass &) = delete;

    struct PosExpr {
    };

    std::vector<std::unique_ptr<PosExpr>> pos_exprs;

    void addPosExpr();
  };

  std::vector<std::unique_ptr<MotionPass>> passes;

  void addMotionPass();
};

#endif /* CHARMAPPER_HPP_ */
