#include "tree.hpp"

#include <cassert>


static void testRemoveChildItems()
{
  Tree tree;
  Tree::Path local_position_path =
    tree.createItem({},TreeItem::Type::local_position);
  tree.createItem(local_position_path,TreeItem::Type::x);
  tree.createItem(local_position_path,TreeItem::Type::y);
  tree.createItem(local_position_path,TreeItem::Type::z);
  assert(tree.nChildItems(local_position_path)==3);
  tree.removeChildItems(local_position_path);
  assert(tree.nChildItems(local_position_path)==0);
}


int main()
{
  testRemoveChildItems();
}
