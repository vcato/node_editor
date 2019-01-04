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
      virtual void notifyCharmapChanged() const = 0;
      virtual void removeCharmapper() const = 0;
    };

    struct WrapperData {
      const SceneList &scene_list;
      ObservedDiagrams &observed_diagrams;
      Callbacks &callbacks;

      void notifyCharmapChanged() const { callbacks.notifyCharmapChanged(); }
      void removeCharmapper() const { callbacks.removeCharmapper(); }

      WrapperData( const SceneList &, ObservedDiagrams &, Callbacks & );
    };

    Charmapper &charmapper;
    const WrapperData &wrapper_data;
    std::string label_member;

    CharmapperWrapper(
      Charmapper &charmapper_arg,
      const WrapperData &wrapper_data_arg,
      const std::string &label_arg
    )
    : charmapper(charmapper_arg),
      wrapper_data(wrapper_data_arg),
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

  static void
    handleSceneChange(
      Charmapper &charmapper,
      const std::string &charmapper_name,
      const TreeObserver &tree_observer,
      const TreePath &charmapper_path,
      ObservedDiagrams &observed_diagrams,
      const SceneList &scene_list
    );

  // May be able to make this private.
  void
    handleSceneChange(
      const TreeObserver &,
      const TreePath &charmapper_path
    );
};
