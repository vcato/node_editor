#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <vector>
#include <memory>
#include "charmapper.hpp"
#include "scene.hpp"
#include "sceneviewer.hpp"


class World {
  public:
    World();
    ~World();

    struct MemberVisitor;

    struct Member {
      virtual void accept(MemberVisitor &) = 0;
      virtual ~Member() {}
    };

    struct CharmapperMember : Member {
      Charmapper charmapper;

      virtual void accept(MemberVisitor &visitor)
      {
        visitor.visitCharmapper(*this);
      }
    };

    struct SceneMember : Member {
      Scene scene;
      SceneViewer *scene_viewer_ptr = nullptr;

      virtual void accept(MemberVisitor &visitor)
      {
        visitor.visitScene(*this);
      }
    };

    struct MemberVisitor {
      virtual void visitCharmapper(CharmapperMember &) = 0;
      virtual void visitScene(SceneMember &) = 0;
    };

    Charmapper& addCharmapper();
    void addScene();
    void visitMember(int child_index,MemberVisitor &);
    int nMembers() const { return world_members.size(); }

  private:
    using WorldMembers = std::vector<std::unique_ptr<Member>>;
    WorldMembers world_members;

    virtual SceneViewer& createSceneViewerWindow(SceneMember &) = 0;
};


#endif /* WORLD_HPP_ */
