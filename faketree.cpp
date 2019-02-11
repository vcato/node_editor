#include "faketree.hpp"

#include "itemfrompath.hpp"
#include "insertitemin.hpp"


FakeTree::Item &
  FakeTree::createItem(
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
  return new_item;
}


void FakeTree::removeItem(const TreePath &path)
{
  Item &parent_item = itemFromPath(root, parentPath(path));
  removeChildItem(parent_item,path.back());
}


void FakeTree::setItemLabel(const TreePath &path,const std::string &new_label)
{
  itemFromPath(root, path).label = new_label;
}
