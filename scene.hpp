#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>
#include <memory>
#include <cassert>
#include "point2d.hpp"
#include "ignore.hpp"


class Scene {
  public:
    struct Body;
    struct Bodies;
    struct Frame;
    using VarIndex = int;
    using VarValue = float;

    Scene();
    ~Scene();

    int nBodies() const { return bodies().size(); }
    Body &addBody();
    Body& addChildBodyTo(Body &parent);
    void removeChildBodyFrom(Body &parent,int child_index);
    const Bodies &bodies() const { return root_body.children; }
    Bodies &bodies() { return root_body.children; }
    Body &body(int index) { return bodies()[index]; }
    Frame makeFrame() const;

  public:
    struct Frame {
      std::vector<float> var_values;
      static float defaultVariableValue() { return 0; }

      Frame(int n_variables = 0)
      : var_values(n_variables,defaultVariableValue())
      {
      }

      int nVariables() const { return var_values.size(); }

      void setNVariables(int arg)
      {
        var_values.resize(arg,defaultVariableValue());
      }
    };

    struct FloatMap {
      VarIndex var_index;

      FloatMap(VarIndex var_index_arg) : var_index(var_index_arg) { }

      VarValue operator()(const Frame &frame) const
      {
        return frame.var_values[var_index];
      }

      void set(Frame &frame,VarValue value)
      {
        frame.var_values[var_index] = value;
      }
    };

    struct Point2DMap {
      FloatMap x;
      FloatMap y;
    };

    class Bodies {
      public:
        using Index = size_t;

        Bodies() = default;

        Bodies(const Bodies &arg);

        Index size() const
        {
          return body_ptrs.size();
        }

        Body& operator[](Index index)
        {
          assert(body_ptrs[index]);
          return *body_ptrs[index];
        }

        const Body& operator[](Index index) const
        {
          assert(body_ptrs[index]);
          return *body_ptrs[index];
        }

        struct const_iterator {
          const Bodies &bodies;
          Index index;

          const_iterator(const Bodies &bodies_arg,Index index_arg)
          : bodies(bodies_arg),
            index(index_arg)
          {
          }

          bool operator!=(const_iterator arg) const
          {
            assert(&bodies==&arg.bodies);
            return index!=arg.index;
          }

          const_iterator& operator++()
          {
            assert(index<bodies.size());
            ++index;
            return *this;
          }

          const Body &operator*() const { return bodies[index]; }
        };

        const_iterator begin() const { return const_iterator(*this,0); }
        const_iterator end() const { return const_iterator(*this,size()); }

      private:
        friend class Scene;

        std::vector<std::unique_ptr<Body>> body_ptrs;

        Body& createChild(const Body& arg)
        {
          body_ptrs.push_back(std::make_unique<Body>(arg));
          return *body_ptrs.back();
        }

        Body& createChild(const Point2DMap &position_map)
        {
          body_ptrs.push_back(std::make_unique<Body>(position_map));
          return *body_ptrs.back();
        }

        void remove(int child_index)
        {
          body_ptrs.erase(body_ptrs.begin() + child_index);
        }
    };

    class Body {
      public:
        Body(const Point2DMap &position_arg) : position(position_arg) { }

        Point2DMap position;
        Bodies children;
        std::string name;

        int nChildren() const { return children.size(); }

      private:
        friend class Scene;

        Body& addChild(const std::string &name,const Point2DMap &position_map)
        {
          Body &result = children.createChild(position_map);
          result.name = name;
          return result;
        }

        void removeChild(int child_index)
        {
          children.remove(child_index);
        }
    };

    int nFrameVariables() const { return n_frame_variables; }
    Frame &backgroundFrame() { return background_frame; }
    const Frame &backgroundFrame() const { return background_frame; }
    const Frame &displayFrame() const { return display_frame; }
    Frame &displayFrame() { return display_frame; }
    Body &rootBody() { return root_body; }

  private:
    int n_frame_variables = 0;
    Body root_body = Body{{0,0}};
    Frame background_frame;
    Frame display_frame;

    std::string newBodyName() const;
    bool hasBody(const Bodies &bodies,const std::string &name) const;
    bool hasBody(const std::string &name) const;
    void addVars(int n_vars);
    Point2DMap newPositionMap();
};

#endif /* SCENE_HPP_ */
