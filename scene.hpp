#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>
#include <set>
#include "point2d.hpp"


class Scene {
  public:
    struct Body {
      Point2D position;
    };

    using Bodies = std::vector<Body>;

    Scene() { }
    ~Scene();

    int nBodies() const { return bodies_member.size(); }

    void addBody();
    const Bodies &bodies() const { return bodies_member; }
    Bodies &bodies() { return bodies_member; }

  private:
    Bodies bodies_member;
};

#endif /* SCENE_HPP_ */
