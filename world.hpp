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
    struct ConstMemberVisitor;

    struct Member {
      virtual void accept(const MemberVisitor &) = 0;
      virtual void acceptConst(const ConstMemberVisitor &) const = 0;
      virtual ~Member() {}
    };

    struct CharmapperMember : Member {
      Charmapper charmapper;

      virtual void accept(const MemberVisitor &visitor)
      {
        visitor.visitCharmapper(*this);
      }

      virtual void acceptConst(const ConstMemberVisitor &visitor) const
      {
        visitor.visitCharmapper(*this);
      }
    };

    struct SceneMember : Member {
      Scene scene;
      SceneViewer *scene_viewer_ptr = nullptr;

      virtual void accept(const MemberVisitor &visitor)
      {
        visitor.visitScene(*this);
      }

      virtual void acceptConst(const ConstMemberVisitor &visitor) const
      {
        visitor.visitScene(*this);
      }
    };

    struct MemberVisitor {
      virtual void visitCharmapper(CharmapperMember &) const = 0;
      virtual void visitScene(SceneMember &) const = 0;
    };

    struct ConstMemberVisitor {
      virtual void visitCharmapper(const CharmapperMember &) const = 0;
      virtual void visitScene(const SceneMember &) const = 0;
    };

    Charmapper& addCharmapper();
    void addScene();
    void visitMember(int child_index,const MemberVisitor &);
    void visitMember(int child_index,const ConstMemberVisitor &) const;
    int nMembers() const { return world_members.size(); }

  private:
    using WorldMembers = std::vector<std::unique_ptr<Member>>;
    WorldMembers world_members;

    virtual SceneViewer& createSceneViewerWindow(SceneMember &) = 0;
};


#endif /* WORLD_HPP_ */
