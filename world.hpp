#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <vector>
#include <memory>
#include <functional>
#include "charmapper.hpp"
#include "scene.hpp"
#include "scenewindow.hpp"
#include "observeddiagrams.hpp"


class World {
  public:
    struct MemberVisitor;
    struct ConstMemberVisitor;
    struct SceneMember;
    struct CharmapperMember;
    struct Member;

    World();
    ~World();

    Charmapper& addCharmapper();
    Scene& addScene();
    std::unique_ptr<Member> removeMember(int member_index);
    int nMembers() const { return world_members.size(); }

    void applyCharmaps();
    void applyCharmaps(const std::vector<Charmapper*> &);

    CharmapperMember &charmapperMember(int member_index);
    SceneMember &sceneMember(int member_index);
    const SceneMember &sceneMember(int member_index) const;
    int memberIndex(const Member &) const;

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
        std::function<void(CharmapperMember &,int member_index)>
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
      struct Listener : SceneListener {
        SceneMember &member;

        Listener(SceneMember &member_arg)
        : member(member_arg)
        {
        }

        void
          frameVariablesChanged(
            int frame_index,
            std::vector<int> &variable_indices
          ) override
        {
          member.frameVariablesChanged(frame_index,variable_indices);
        }
      };

      Scene scene;
      SceneWindow *scene_window_ptr = nullptr;
      Listener listener;
      World &world;

      SceneMember(World &world_arg)
      : listener(*this),
        world(world_arg)
      {
      }

      virtual void accept(const MemberVisitor &visitor)
      {
        visitor.visitScene(*this);
      }

      virtual void acceptConst(const ConstMemberVisitor &visitor) const
      {
        visitor.visitScene(*this);
      }

      void frameVariablesChanged(
        int frame_index,
        std::vector<int> &variable_indices
      )
      {
        world.sceneMemberFrameVariblesChanged(
          *this,frame_index,variable_indices
        );
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

    using SceneFrameVariablesChangedFunctionType =
      void (
        int scene_member_index,
        int frame_index,
        const std::vector<int> &variable_indices
      );

    ObservedDiagrams observed_diagrams;
    std::function<SceneFrameVariablesChangedFunctionType>
      scene_frame_variables_changed_function;

  private:
    using WorldMembers = std::vector<std::unique_ptr<Member>>;

    WorldMembers world_members;

    const Member* findMember(const std::string &name) const;
    virtual SceneWindow& createSceneViewerWindow(SceneMember &) = 0;
    virtual void destroySceneViewerWindow(SceneWindow &) = 0;
    std::string generateMemberName(const std::string &prefix) const;

    void
      forEachMember(
        const std::function<void(const Member &,int member_index)> &f
      ) const;

    template <typename TypedMember>
    void
      forEachMemberIndexOfType(
        std::function<void(int member_index)> f
      ) const
    {
      forEachMember([&](const Member &member,int member_index){
        const Member *member_ptr = &member;
        assert(member_ptr);

        if (dynamic_cast<const TypedMember*>(member_ptr)) {
          f(member_index);
        }
      });
    }

    template <typename TypedMember>
    const TypedMember &typedMember(int member_index) const
    {
      const Member *member_ptr = world_members[member_index].get();
      assert(member_ptr);
      const TypedMember* typed_member_ptr =
        dynamic_cast<const TypedMember*>(member_ptr);
      assert(typed_member_ptr);
      return *typed_member_ptr;
    }

    template <typename TypedMember>
    void
      forEachMemberOfType(
        std::function<void(const TypedMember &,int member_index)> f
      ) const
    {
      auto callWithMember =
        [&](int member_index){
          f(typedMember<TypedMember>(member_index),member_index);
        };

      forEachMemberIndexOfType<TypedMember>(callWithMember);
    }

    template <typename TypedMember>
    void
      forEachMemberOfType(
        std::function<void(TypedMember &,int member_index)> function_to_call
      )
    {
      const World &const_world = *this;
      const_world.forEachMemberOfType<TypedMember>(
        [&](const TypedMember &const_member,int member_index)
        {
          function_to_call(
            const_cast<TypedMember&>(const_member),
            member_index
          );
        }
      );
    }

    std::vector<Charmapper*> allCharmapPtrs();
    void notifyDiagramChanged(const Diagram &);

    void
      sceneMemberFrameVariblesChanged(
        SceneMember &scene_member,
        int frame_index,
        const std::vector<int> &variable_indices
      );
};


#endif /* WORLD_HPP_ */
