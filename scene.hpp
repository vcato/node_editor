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

    static Body& createChild(Bodies &bodies)
    {
      return bodies.createChild();
    }

    static Body& child(Bodies &bodies,int index)
    {
      return bodies[index];
    }

    struct Body {
      Point2D position;
      Bodies children;
      std::string name;

      Body& addChild() { return createChild(children); }
      int nChildren() const { return children.size(); }
    };

    Scene() { }
    ~Scene();

    int nBodies() const { return bodies_member.size(); }

    Body &addBody();
    void addChildBodyTo(Body &parent);
    const Bodies &bodies() const { return bodies_member; }
    Bodies &bodies() { return bodies_member; }
    Body &body(int index) { return child(bodies_member,index); }

  private:
    Bodies bodies_member;

    std::string newBodyName() const;
    bool hasBody(const Bodies &bodies,const std::string &name) const;
    bool hasBody(const std::string &name) const;
};

#endif /* SCENE_HPP_ */
