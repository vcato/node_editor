struct Scene {
  struct Body {
  };

  std::vector<Body> bodies;

  void addBody()
  {
    bodies.push_back(Body());
  }
};
