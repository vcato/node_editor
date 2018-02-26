#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>
#include <memory>
#include <cassert>
#include "diagram.hpp"
#include "scene.hpp"


class Charmapper {
  public:
    struct MotionPass;

    Charmapper() = default;
    Charmapper(const Charmapper &) = delete;

#if USE_POINT2D_MAP
    void apply(Scene::Frame &);
#else
    void apply();
#endif

    int nPasses() const { return passes.size(); }

    MotionPass &pass(int pass_index)
    {
      assert(passes[pass_index]);
      return *passes[pass_index];
    }

    MotionPass& addMotionPass();

  public:
    struct Channel {
      Diagram diagram;
      int value = 0;
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
        Scene::Body *source_body_ptr;

        FromBodyData();

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

      void switchToComponents();

      void switchToFromBody();

      bool isComponents() const
      {
        if (dynamic_cast<ComponentsData*>(global_position_ptr.get())) {
          return true;
        }

        assert(false);
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
    };

    struct MotionPass {
      MotionPass();
      MotionPass(const MotionPass &) = delete;

      struct PosExpr {
        Diagram diagram;
        Position local_position;
        GlobalPosition global_position;
        std::string target_body_name;

        bool hasATargetBody() const
        {
          return target_body_ptr!=nullptr;
        }

        void setTargetBodyPtr(Scene::Body *arg)
        {
          target_body_ptr = arg;
        }

        PosExpr();

        Scene::Body *target_body_ptr = nullptr;
      };

      PosExpr &expr(int index)
      {
        assert(pos_exprs[index]);
        return *pos_exprs[index];
      }

      int nExprs() const { return pos_exprs.size(); }

      PosExpr& addPosExpr();

      private:
        std::vector<std::unique_ptr<PosExpr>> pos_exprs;

    };

  private:
    std::vector<std::unique_ptr<MotionPass>> passes;
};

#endif /* CHARMAPPER_HPP_ */
