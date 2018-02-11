#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>
#include "point2d.hpp"


class Scene {
  public:
    struct Body;

    using Bodies = std::vector<Body>;

    struct Body {
      Point2D position;
      Bodies children;
      std::string name;

      void addChild() { children.emplace_back(); }
      int nChildren() const { return children.size(); }
    };

    Scene() { }
    ~Scene();

    int nBodies() const { return bodies_member.size(); }

    void addBody();
    void addChildBodyTo(Body &parent);
    const Bodies &bodies() const { return bodies_member; }
    Bodies &bodies() { return bodies_member; }

  private:
    Bodies bodies_member;

    std::string newBodyName() const;
    bool hasBody(const std::vector<Body> &bodies,const std::string &name) const;
    bool hasBody(const std::string &name) const;
};

#endif /* SCENE_HPP_ */
