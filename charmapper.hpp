#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>
#include <cassert>
#include "diagram.hpp"
#include "scene.hpp"
#include "bodylink.hpp"
#include "diagramexecutioncontext.hpp"
#include "abstractdiagramevaluator.hpp"


class Charmapper {
  public:
    struct Pass;
    struct MotionPass;
    struct VariablePass;

    Charmapper() = default;
    Charmapper(const Charmapper &) = delete;

    void apply(AbstractDiagramEvaluator &evaluator);
    int nPasses() const { return passes.size(); }
    MotionPass *maybeMotionPass(int pass_index);
    VariablePass *maybeVariablePass(int pass_index);
    const VariablePass *maybeVariablePass(int pass_index) const;
    const MotionPass *maybeMotionPass(int pass_index) const;
    MotionPass &motionPass(int pass_index);
    MotionPass& addMotionPass();
    VariablePass& addVariablePass();
    void removePass(int pass_index);
    void insertVariablePass(int pass_index);

  public:

    struct Channel {
      using Value = int;
      // We'll want to have Channels have diagrams at some point.
      Value value = 0;
      Optional<Diagram> optional_diagram;
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

      GlobalPosition();

      struct Data {
        struct Visitor {
          virtual void visit(FromBodyData &) const = 0;
          virtual void visit(ComponentsData &) const = 0;
        };

        virtual void accept(const Visitor &) = 0;
      };

      struct FromBodyData : Data {
        Position local_position;
        BodyLink source_body_link;

        FromBodyData();

        virtual void accept(const Visitor &visitor)
        {
          visitor.visit(*this);
        }

        static const Diagram& defaultLocalPositionDiagram();
      };

      struct ComponentsData : Data {
        Channel x,y;

        virtual void accept(const Visitor &visitor)
        {
          visitor.visit(*this);
        }
      };

      void switchToComponents();

      void switchToFromBody();

      bool isComponents() const
      {
        if (dynamic_cast<ComponentsData*>(global_position_ptr.get())) {
          return true;
        }

        return false;
      }

      bool isFromBody() const
      {
        if (dynamic_cast<FromBodyData*>(global_position_ptr.get())) {
          return true;
        }

        return false;
      }

      FromBodyData &fromBody()
      {
        FromBodyData *data_ptr =
          dynamic_cast<FromBodyData*>(global_position_ptr.get());
        assert(data_ptr);
        return *data_ptr;
      }

      ComponentsData &components()
      {
        ComponentsData *data_ptr =
          dynamic_cast<ComponentsData*>(global_position_ptr.get());
        assert(data_ptr);
        return *data_ptr;
      }

      static const Diagram& defaultComponentsDiagram();
      static const Diagram& defaultFromBodyDiagram();
    };

    struct Pass {
      virtual ~Pass() = default;
    };

    struct MotionPass : Pass {
      MotionPass();
      MotionPass(const MotionPass &) = delete;
      
      struct PosExpr {
        PosExpr();

        bool hasATargetBody() const
        {
          return target_body_link.hasValue();
        }

        static const Diagram &defaultLocalPositionDiagram();
        static const Diagram &defaultDiagram();

        Diagram diagram;
        Position local_position;
        GlobalPosition global_position;
        BodyLink target_body_link;
      };

      PosExpr &expr(int index)
      {
        assert(pos_exprs[index]);
        return *pos_exprs[index];
      }

      int nExprs() const { return pos_exprs.size(); }

      PosExpr& addPosExpr();

      void removePosExpr(int index);

      private:
        std::vector<std::unique_ptr<PosExpr>> pos_exprs;

    };

    struct Variable {
      using Name = std::string;
      Name name;

      Variable(const Name &name_arg) : name(name_arg) { }

      Channel value;
    };

    struct VariablePass : Pass {
      using Variables = std::vector<Variable>;
      using VariableIndex = int;
      Variables variables;

      VariableIndex addVariable(const Variable::Name &name)
      {
        VariableIndex new_variable_index = variables.size();

        variables.push_back(Variable{name});

        return new_variable_index;
      }
    };

  private:
    std::vector<std::unique_ptr<Pass>> passes;
};

#endif /* CHARMAPPER_HPP_ */
