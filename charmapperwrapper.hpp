#include "charmapper.hpp"
#include "wrapper.hpp"
#include "wrapperstate.hpp"
#include "observeddiagrams.hpp"


class CharmapperWrapper : public VoidWrapper {
  public:
    struct SceneList {
      using BodyLinks = std::vector<BodyLink>;

      virtual int nScenes() = 0;
      virtual std::vector<std::string> allBodyNames() const = 0;
      virtual BodyLinks allBodyLinks() const = 0;
    };

    struct Callbacks {
      const SceneList &scene_list;
      ObservedDiagrams &observed_diagrams;
      virtual void notifyCharmapChanged() const = 0;
      virtual void removeCharmapper() const = 0;

      Callbacks(
        const SceneList &scene_list_arg,
        ObservedDiagrams &observed_diagrams_arg
      );
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
        const TreePath &,
        TreeObserver &
      ) const override;

    void setState(const WrapperState &) const override;
    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const;
    Label label() const override;

    virtual int nChildren() const
    {
      return charmapper.nPasses();
    }

    void
      handleSceneChange(
        const TreeObserver &,
        const TreePath &charmapper_path
      );
};
