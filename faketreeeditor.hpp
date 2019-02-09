#include "treeeditor.hpp"
#include "faketree.hpp"
#include "itemfrompath.hpp"
#include "wrapperutil.hpp"
#include "fakediagrameditorwindows.hpp"

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

  template <typename T>
  static T& insertItemIn(std::vector<T> &container,size_t index)
  {
    container.emplace(container.begin() + index);
    FakeTree::Item &new_item = container[index];
    return new_item;
  }


  static void
    createItem(
      FakeTree &tree,
      const TreePath &new_item_path,
      const LabelProperties &label_properties
    )
  {
    TreePath parent_path = parentPath(new_item_path);
    FakeTree::Item &parent_item = itemFromPath(tree.root, parent_path);
    int new_item_index = new_item_path.back();
    FakeTree::Item &new_item =
      insertItemIn(parent_item.children,new_item_index);
    new_item.label = label_properties.text;
    new_item.label_is_editable = label_properties.is_editable;
  }

  void
    createVoidItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties
    ) override
  {
    createItem(
      tree,
      new_item_path,
      label_properties
    );
  }

  void
    createNumericItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties,
      const NumericValue /*value*/,
      const NumericValue /*minimum_value */,
      const NumericValue /*maximum_value */
    ) override
  {
    createItem(tree,new_item_path,label_properties);
  }

  void
    createEnumerationItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties,
      const std::vector<std::string> &/*options*/,
      int /*value*/
    ) override
  {
    createItem(tree,new_item_path,label_properties);
  }

  void
    createStringItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties,
      const std::string &/*value*/
    ) override
  {
    createItem(tree,new_item_path,label_properties);
  }

  void setItemLabel(const TreePath &path,const std::string &new_label)
  {
    itemFromPath(tree.root, path).label = new_label;
  }

  FakeTree tree;
  FakeDiagramEditorWindows diagram_editor_windows;
};
