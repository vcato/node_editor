#ifndef CHARMAPPER_HPP_
#define CHARMAPPER_HPP_

#include <vector>
#include <map>
#include <memory>
#include <cassert>
#include "diagram.hpp"
#include "scene.hpp"


class Charmapper {
  public:
    struct MotionPass;

    Charmapper() = default;
    Charmapper(const Charmapper &) = delete;

    void apply();

    int nPasses() const { return passes.size(); }

    MotionPass &pass(int pass_index)
    {
      assert(passes[pass_index]);
      return *passes[pass_index];
    }

    MotionPass& addMotionPass();

    void removePass(int pass_index);

  public:
    class BodyLink {
      public:
        BodyLink(Scene *scene_ptr_arg,Scene::Body *body_ptr_arg)
        : scene_ptr(scene_ptr_arg),
          body_ptr(body_ptr_arg)
        {
        }

        BodyLink() = default;

        Scene &scene()
        {
          assert(scene_ptr);
          return *scene_ptr;
        }

        void set(Scene *scene_ptr_arg,Scene::Body *body_ptr_arg)
        {
          assert(!body_ptr_arg || scene_ptr_arg);
          scene_ptr = scene_ptr_arg;
          body_ptr = body_ptr_arg;
        }

        bool hasValue() const
        {
          return body_ptr!=nullptr;
        }

        Scene::Body *bodyPtr() const
        {
          return body_ptr;
        }

        Scene::Body &body() const
        {
          assert(body_ptr);
          return *body_ptr;
        }

        void clear()
        {
          *this = BodyLink();
        }

        bool operator==(const BodyLink &arg) const
        {
          return scene_ptr==arg.scene_ptr && body_ptr==arg.body_ptr;
        }

      private:
        Scene *scene_ptr = nullptr;
        Scene::Body *body_ptr = nullptr;
    };

    struct Channel {
      using Value = int;
      Diagram diagram;
      Value value = 0;
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
    };

    struct MotionPass {
      MotionPass();
      MotionPass(const MotionPass &) = delete;
      
      struct PosExpr {
        PosExpr();

        bool hasATargetBody() const
        {
          return target_body_link.hasValue();
        }

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

  private:
    std::vector<std::unique_ptr<MotionPass>> passes;
};

#endif /* CHARMAPPER_HPP_ */
