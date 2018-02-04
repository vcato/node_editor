#include "scene.hpp"
#include "tree.hpp"


struct SceneWrapper : Wrapper {
  Scene &scene;

  SceneWrapper(Scene &scene_arg)
  : scene(scene_arg)
  {
  }

  void
    visitOperations(
      const TreePath &path,
      const TreeItem::OperationVisitor &visitor
    ) const;

  void
    visitWrapper(
      const TreePath &,
      int depth,
      const WrapperVisitor &
    );
};
