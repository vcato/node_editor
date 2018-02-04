struct Charmapper {
  struct MotionPass {
  };

  std::vector<MotionPass> passes;

  void addMotionPass()
  {
    passes.push_back(MotionPass());
  }
};
