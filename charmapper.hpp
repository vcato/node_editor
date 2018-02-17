#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>
#include <memory>
#include <cassert>
#include "diagram.hpp"
#include "defaultdiagrams.hpp"


struct Charmapper {
  Charmapper() = default;
  Charmapper(const Charmapper &) = delete;

  struct Channel {
    Diagram diagram;
  };

  struct Position {
    Diagram diagram;
    Channel x,y;

    Position(const Diagram &diagram_arg)
    : diagram(diagram_arg)
    {
    }
  };

  struct GlobalPosition {
    struct FromBodyData;
    struct ComponentsData;
    struct Data;
    Diagram diagram;
    std::unique_ptr<Data> global_position_ptr;

    GlobalPosition()
    : diagram(fromComponentsDiagram()),
      global_position_ptr(std::make_unique<ComponentsData>())
    {
    }

    struct Data {
      struct Visitor {
        virtual void visit(FromBodyData &) const = 0;
        virtual void visit(ComponentsData &) const = 0;
      };

      virtual void accept(const Visitor &) = 0;
    };

    struct FromBodyData : Data {
      Position local_position;

      FromBodyData()
      : local_position(localPositionDiagram())
      {
      }

      virtual void accept(const Visitor &visitor)
      {
        visitor.visit(*this);
      }
    };

    struct ComponentsData : Data, Position {
      ComponentsData()
      : Position(Diagram())
      {
      }

      virtual void accept(const Visitor &visitor)
      {
        visitor.visit(*this);
      }
    };

    void switchToComponents()
    {
      diagram = fromComponentsDiagram();
      global_position_ptr = std::make_unique<ComponentsData>();
    }

    void switchToFromBody()
    {
      diagram = fromBodyDiagram();
      global_position_ptr = std::make_unique<FromBodyData>();
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
      : diagram(posExprDiagram()),
        local_position(localPositionDiagram())
      {
      }
    };

    std::vector<std::unique_ptr<PosExpr>> pos_exprs;

    int nExprs() const { return pos_exprs.size(); }
    PosExpr& addPosExpr();
  };

  std::vector<std::unique_ptr<MotionPass>> passes;

  int nPasses() const { return passes.size(); }
  MotionPass& addMotionPass();
};

#endif /* CHARMAPPER_HPP_ */
