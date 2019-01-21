#include "treeeditor.hpp"
#include "faketree.hpp"
#include "itemfrompath.hpp"
#include "wrapperutil.hpp"

struct FakeTreeEditor : TreeEditor {
  using Item = FakeTreeItem;

  Item &root()
  {
    return tree.root;
  }

  const Item &root() const
  {
    return tree.root;
  }

  void removeTreeItem(const TreePath &path) override
  {
    Item &parent_item = itemFromPath(root(), parentPath(path));
    removeChildItem(parent_item,path.back());
  }

  void
    userSelectsContextMenuItem(
      const TreePath &path,
      const std::string &operation_name
    )
  {
    if (operation_name=="Edit Diagram...") {
      openDiagramEditor(path);
    }
    else {
      executeOperation(path,operationIndex2(world(),path,operation_name));
    }
  }

  void
    userSelectsContextMenuItem(
      const std::string &path_string,
      const std::string &operation_name
    )
  {
    TreePath path = makePath(world(),path_string);
    userSelectsContextMenuItem(path,operation_name);
  }


  void userSelectsContextMenuItem(const std::string &operation_name);

  void removeChildItems(const TreePath &path) override
  {
    Item &parent_item = itemFromPath(root(),path);
    parent_item.children.clear();
  }

  FakeTree tree;
};
