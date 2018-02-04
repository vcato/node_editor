#include "scene.hpp"
#include "tree.hpp"


struct SceneWrapper {
  Scene &scene;

  bool
    visitOperations(
      const TreePath &path,
      int depth,
      const TreeItem::OperationVisitor &visitor
    );
};
