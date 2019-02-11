#include "treeeditor.hpp"
#include "faketree.hpp"
#include "itemfrompath.hpp"
#include "wrapperutil.hpp"
#include "fakediagrameditorwindows.hpp"
#include "insertitemin.hpp"

struct FakeTreeEditor : TreeEditor {
  using Item = FakeTreeItem;

  Item &root()
  {
    return tree_member.root;
  }

  const Item &root() const
  {
    return tree_member.root;
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

  void
    setItemExpanded(
      const TreePath &path,
      bool new_expanded_state
    ) override
  {
    itemFromPath(root(),path).is_expanded = new_expanded_state;
  }

  void userChangesStringValue(const TreePath &path,const std::string &new_value)
  {
    stringItemValueChanged(path,new_value);
  }

  void
    userChangesStringValue(
      const std::string &path_string,
      const std::string &new_value
    )
  {
    userChangesStringValue(makePath(world(),path_string),new_value);
  }

  void userChangesNumberValue(const TreePath &path,int new_value)
  {
    numberItemValueChanged(path,new_value);
  }

  void userChangesNumberValue(const std::string &path_string,int new_value)
  {
    userChangesNumberValue(makePath(world(),path_string),new_value);
  }

  void userChangesComboBoxIndex(const std::string &path_string,int new_value)
  {
    setEnumerationIndex(makePath(world(),path_string),new_value);
  }

  int itemChildCount(const TreePath &parent_path) const override
  {
    return itemFromPath(root(),parent_path).children.size();
  }

  DiagramEditorWindow& createDiagramEditor() override
  {
    return diagram_editor_windows.create();
  }

  static FakeTree::Item &
    createItem(
      FakeTree &tree,
      const TreePath &new_item_path,
      const LabelProperties &label_properties
    )
  {
    return FakeTree::createItem(tree,new_item_path,label_properties);
  }

  TreeWidget &tree() override { return tree_member; }

  FakeTree tree_member;
  FakeDiagramEditorWindows diagram_editor_windows;
};
