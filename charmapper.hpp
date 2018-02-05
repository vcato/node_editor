#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>
#include <memory>
#include <cassert>
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

  struct GlobalPosition {
    struct FromBodyData;
    struct FromComponentsData;
    Diagram diagram;

    struct Data {
      struct Visitor {
        virtual void accept(FromBodyData &) const = 0;
        virtual void accept(FromComponentsData &) const = 0;
      };

      virtual void accept(const Visitor &) = 0;
    };

    struct FromBodyData : Data {
      Position local_position;

      virtual void accept(const Visitor &visitor)
      {
        visitor.accept(*this);
      }
    };

    struct FromComponentsData : Data, Position {
      virtual void accept(const Visitor &visitor)
      {
        visitor.accept(*this);
      }
    };

    std::unique_ptr<Data> global_position_ptr;

    void switchToFromComponents()
    {
      global_position_ptr = std::make_unique<FromComponentsData>();
    }

    void switchToFromBody()
    {
      global_position_ptr = std::make_unique<FromBodyData>();
    }

    GlobalPosition()
      : global_position_ptr(
          std::make_unique<FromComponentsData>()
        )
    {
    }
  };

  struct MotionPass {
    MotionPass();
    MotionPass(const MotionPass &) = delete;

    struct PosExpr {
      Diagram diagram;
      Position local_position;
      GlobalPosition global_position;

      PosExpr()
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
