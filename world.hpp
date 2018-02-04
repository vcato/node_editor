#include <vector>
#include "tree.hpp"

struct Charmapper {
  struct MotionPass {
  };

  std::vector<MotionPass> passes;

  void addMotionPass()
  {
    passes.push_back(MotionPass());
  }
};


struct Scene {
  struct Body {
  };

  std::vector<Body> bodies;

  void addBody()
  {
    bodies.push_back(Body());
  }
};


class World : public WorldInterface {
  public:
    struct WorldMember;

    World();
    ~World();

    virtual bool
      visitOperations(
        const TreePath &path,int depth,const OperationVisitor &visitor
      );

  private:
    std::vector<std::unique_ptr<WorldMember>> world_members;

    void addCharmapper();
    void addScene();

    virtual void createSceneWindow() = 0;
};
