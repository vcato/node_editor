#ifndef WORLD_HPP_
#define WORLD_HPP_

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


class World {
  public:
    World();
    ~World();

    struct MemberVisitor {
      virtual void visitCharmapper(Charmapper &) = 0;
      virtual void visitScene(Scene &) = 0;
    };

    void addCharmapper();
    void addScene();
    void visitMember(int child_index,MemberVisitor &);

    struct WorldMember;
    using WorldMembers = std::vector<std::unique_ptr<WorldMember>>;
    WorldMembers world_members;

  private:

    virtual void createSceneWindow() = 0;
};


#include "worldwrapper.hpp"

#endif /* WORLD_HPP_ */
