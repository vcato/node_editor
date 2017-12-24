#include "tree.hpp"

#include <cassert>


static void testRemoveChildItems()
{
  Tree tree;
  Tree::Path local_position_path = tree.createLocalPositionItem({});
  tree.createXItem(local_position_path);
  tree.createYItem(local_position_path);
  tree.createZItem(local_position_path);
  assert(tree.nChildItems(local_position_path)==3);
  tree.removeChildItems(local_position_path);
  assert(tree.nChildItems(local_position_path)==0);
}


int main()
{
  testRemoveChildItems();
}
