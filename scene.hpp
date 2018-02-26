#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>
#include <memory>
#include <cassert>
#include "point2d.hpp"
#include "ignore.hpp"


#define USE_FRAMES 0


class Scene {
  public:
    struct Body;
    struct Bodies;
    struct Frame;
    using VarIndex = int;
    using VarValue = float;

    Scene() { }
    ~Scene();

    int nBodies() const { return bodies_member.size(); }
    Body &addBody();
    void addChildBodyTo(Body &parent);
    const Bodies &bodies() const { return bodies_member; }
    Bodies &bodies() { return bodies_member; }
    Body &body(int index) { return bodies_member[index]; }
    Frame makeFrame() const;

  public:
    struct Frame {
      std::vector<float> var_values;
    };

    struct Bodies {
      std::vector<std::unique_ptr<Body>> body_ptrs;
      using Index = size_t;

      Bodies() = default;

      Bodies(const Bodies &arg);

      Body& createChild(const Body& arg)
      {
        body_ptrs.push_back(std::make_unique<Body>(arg));
        return *body_ptrs.back();
      }

      Body& createChild()
      {
        body_ptrs.push_back(std::make_unique<Body>());
        return *body_ptrs.back();
      }

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
    };

    struct FloatMap {
      VarIndex var_index;

      VarValue operator()(const Frame &) const { assert(false); }
    };

#if USE_FRAMES
    struct Point2DMap {
      FloatMap x;
      FloatMap y;
    };
#endif

    struct Body {
#if !USE_FRAMES
      Point2D position;
#else
      Point2DMap position;
#endif
      Bodies children;
      std::string name;

      int nChildren() const { return children.size(); }

      Body& addChild(const std::string &name)
      {
        Body &result = children.createChild();
        result.name = name;
        return result;
      }
    };

#if USE_FRAMES
    Frame &backgroundFrame() { return background_frame; }
#endif

  private:
    Bodies bodies_member;
#if USE_FRAMES
    Frame background_frame;
#endif

    std::string newBodyName() const;
    bool hasBody(const Bodies &bodies,const std::string &name) const;
    bool hasBody(const std::string &name) const;
};

#endif /* SCENE_HPP_ */
