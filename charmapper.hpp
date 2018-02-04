#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>
#include <memory>
#include "diagram.hpp"


struct Charmapper {
  Charmapper() = default;
  Charmapper(const Charmapper &) = delete;

  struct Channel {
    Diagram diagram;
  };

  struct Position {
    Diagram diagram;
    Channel x,y,z;
  };

  struct FromBodyGlobalPosition;
  struct FromComponentsGlobalPosition;

  struct GlobalPosition {
    struct Visitor {
      virtual void accept(FromBodyGlobalPosition &) const = 0;
      virtual void accept(FromComponentsGlobalPosition &) const = 0;
    };

    virtual void accept(const Visitor &) = 0;
  };

  struct FromBodyGlobalPosition : GlobalPosition {
  };

  struct FromComponentsGlobalPosition : GlobalPosition, Position {
    virtual void accept(const Visitor &visitor)
    {
      visitor.accept(*this);
    }
  };

  struct MotionPass {
    MotionPass();
    MotionPass(const MotionPass &) = delete;

    struct PosExpr {
      Diagram diagram;
      Position local_position;
      std::unique_ptr<GlobalPosition> global_position_ptr;

      PosExpr()
      : global_position_ptr(std::make_unique<FromComponentsGlobalPosition>())
      {
      }
    };

    std::vector<std::unique_ptr<PosExpr>> pos_exprs;

    void addPosExpr();
  };

  std::vector<std::unique_ptr<MotionPass>> passes;

  void addMotionPass();
};

#endif /* CHARMAPPER_HPP_ */
