#include "charmapper.hpp"
#include "wrapper.hpp"


class CharmapperWrapper : public VoidWrapper {
  public:
    struct SceneList {
      // using State = std::vector<Scene>;
      // State state() const;
      // void setState(const State &);
      virtual int nScenes() = 0;
      virtual std::vector<std::string> allBodyNames() const = 0;
      using BodyLink = Charmapper::BodyLink;
      using BodyLinks = std::vector<BodyLink>;
      virtual BodyLinks allBodyLinks() const = 0;
    };

    struct Callbacks {
      const SceneList &scene_list;
      virtual void notifyCharmapChanged() const = 0;

      Callbacks(const SceneList &scene_list_arg)
      : scene_list(scene_list_arg)
      {
      }
    };

    Charmapper &charmapper;
    const Callbacks &callbacks;
    std::string label_member;

    CharmapperWrapper(
      Charmapper &charmapper_arg,
      const Callbacks &callbacks_arg,
      const std::string &label_arg
    )
    : charmapper(charmapper_arg),
      callbacks(callbacks_arg),
      label_member(label_arg)
    {
    }

    std::vector<std::string> operationNames() const;

    void
      executeOperation(
        int operation_index,
        const TreePath &path,
        OperationHandler &handler
      ) const override;

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const;

    Label label() const override;

    virtual int nChildren() const
    {
      return charmapper.nPasses();
    }

    void
      handleSceneChange(
        const OperationHandler &,
        const TreePath &charmapper_path
      );
};
