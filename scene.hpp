#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>


struct Scene {
  struct Body {
  };

  std::vector<Body> bodies;

  void addBody()
  {
    bodies.push_back(Body());
  }
};

#endif /* SCENE_HPP_ */
