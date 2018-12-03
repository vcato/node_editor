#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <vector>
#include <memory>
#include <functional>
#include "charmapper.hpp"
#include "scene.hpp"
#include "scenewindow.hpp"


class World {
  public:
    struct MemberVisitor;
    struct ConstMemberVisitor;
    struct SceneMember;
    struct CharmapperMember;

    World();
    ~World();

    Charmapper& addCharmapper();
    Scene& addScene();
    void removeMember(int member_index);
    int nMembers() const { return world_members.size(); }
    void applyCharmaps();
    void applyCharmaps(const std::vector<Charmapper*> &);
    CharmapperMember &charmapperMember(int member_index);
    SceneMember &sceneMember(int member_index);

    void
      forEachSceneMember(
        std::function<void(const SceneMember &)>
      ) const;

    void
      forEachSceneMember(
        std::function<void(SceneMember &)>
      );

    void
      forEachCharmapperMember(
        std::function<void(const CharmapperMember &)>
      ) const;

    void
      forEachCharmapperMember(
        std::function<void(CharmapperMember &)>
      );

    void visitMember(int child_index,const MemberVisitor &);
    void visitMember(int child_index,const ConstMemberVisitor &) const;

  public:
    struct Member {
      std::string name;

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
      SceneWindow *scene_window_ptr = nullptr;

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

  private:
    const Member* findMember(const std::string &name) const;

  private:
    using WorldMembers = std::vector<std::unique_ptr<Member>>;
    WorldMembers world_members;

    virtual SceneWindow& createSceneViewerWindow(SceneMember &) = 0;
    std::string generateMemberName(const std::string &prefix) const;

    template <typename TypedMember>
    void
      forEachMemberOfType(
        std::function<void(const TypedMember &scene)> f
      ) const
    {
      int n_members = nMembers();

      for (int i=0; i!=n_members; ++i) {
        const Member *member_ptr = world_members[i].get();
        assert(member_ptr);
        const TypedMember* typed_member_ptr =
          dynamic_cast<const TypedMember*>(member_ptr);

        if (typed_member_ptr) {
          f(*typed_member_ptr);
        }
      }
    }

    template <typename TypedMember>
    void
      forEachMemberOfType(
        std::function<void(TypedMember &scene)> function_to_call
      )
    {
      const World &const_world = *this;
      const_world.forEachMemberOfType<TypedMember>(
        [&](const TypedMember &const_member)
        {
          function_to_call(const_cast<TypedMember&>(const_member));
        }
      );
    }

    std::vector<Charmapper*> allCharmapPtrs();
};


#endif /* WORLD_HPP_ */
