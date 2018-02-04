#include "charmapper.hpp"
#include "tree.hpp"


struct CharmapperWrapper {
  Charmapper &charmapper;

  bool
    visitOperations(
      const TreePath &path,
      int depth,
      const TreeItem::OperationVisitor &visitor
    );
};
