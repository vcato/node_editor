#ifndef TREEUPDATING_HPP_
#define TREEUPDATING_HPP_


#include "treewidget.hpp"
#include "wrapper.hpp"

extern void
  addChildTreeItems(
    TreeWidget &tree,
    const Wrapper &world,
    const TreePath &parent_path
  );


struct TreeUpdatingObserver : ::TreeObserver {
  using ItemRemovedFunction = std::function<void(const TreePath &)>;

  TreeWidget &tree;
  Wrapper &world;
  ItemRemovedFunction item_removed_function;

  TreeUpdatingObserver(
    TreeWidget &tree_arg,
    Wrapper &world_arg,
    ItemRemovedFunction item_removed_function_arg
  )
  : tree(tree_arg),
    world(world_arg),
    item_removed_function(item_removed_function_arg)
  {
  }

  void itemAdded(const TreePath &path) override;
  void itemReplaced(const TreePath &path) override;
  void itemRemoved(const TreePath &path) override;
  void itemLabelChanged(const TreePath &path) override;
  void itemValueChanged(const TreePath &path) override;
};


#endif /* TREEUPDATING_HPP_ */
