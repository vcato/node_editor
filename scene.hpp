#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>


struct Scene {
  struct Body {
  };

  std::vector<Body> bodies;

  int nBodies() const { return bodies.size(); }

  void addBody()
  {
    bodies.push_back(Body());
  }
};

#endif /* SCENE_HPP_ */
