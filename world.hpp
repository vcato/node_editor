#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <vector>
#include <memory>
#include "charmapper.hpp"
#include "scene.hpp"


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


#endif /* WORLD_HPP_ */
