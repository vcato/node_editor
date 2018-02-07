#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include "defaultdiagrams.hpp"
#include "streamvector.hpp"


using std::string;
using std::cerr;
using std::function;
using std::vector;

using Path = Tree::Path;
using OperationVisitor = Tree::OperationVisitor;
using TreeItem = Tree::Item;


void Tree::Item::createItem(Index index)
{
  Index n_children = child_items.size();

  if (index!=n_children) {
    // Haven't implemented the case where we are inserting an item at
    // some location other than the end.
    assert(false);
  }

  child_items.push_back(TreeItem());
}


Tree::Tree()
{
}


void Tree::createItem(const Path &path)
{
  Path parent_path = parentPath(path);
  Index child_index = path.back();
  Item &parent_item = getItem(parent_path);
  parent_item.createItem(child_index);
}


auto Tree::getItem(const Path &path) -> Item&
{
  const Tree &const_self = *this;
  const Item &const_node = const_self.getItem(path);
  return const_cast<Item&>(const_node);
}


auto Tree::getItem(const Path &path) const -> const Item &
{
  const TreeItem *item_ptr = &_root_item;
  int path_length = path.size();

  int depth = 0;

  while (depth<path_length) {
    item_ptr = &item_ptr->child_items[path[depth]];
    ++depth;
  }

  return *item_ptr;
}


void Tree::visitWrapper(const Path &path,const WrapperVisitor &visitor)
{
  world().visitWrapper(path,/*depth*/0,visitor);
}


Diagram *Tree::itemDiagramPtr(const Path &path)
{
  Diagram *result_ptr = 0;

  visitWrapper(
    path,
    [&result_ptr](const Wrapper &wrapper){ result_ptr = wrapper.diagramPtr(); }
  );

  if (!result_ptr) {
    cerr << "No diagram found for " << path << "\n";
    return nullptr;
  }

  return result_ptr;
}


void Tree::removeChildItems(const Path &path)
{
  getItem(path).child_items.clear();
}


auto Tree::nChildItems(const Path &path) const -> SizeType
{
  const TreeItem &item = getItem(path);
  assert(item.child_items.size()<=std::numeric_limits<SizeType>::max());
  return item.child_items.size();
}


void Tree::visitOperations(const Path &path,const OperationVisitor &visitor)
{
  visitWrapper(
    path,
    [&](const Wrapper &wrapper){
      wrapper.visitOperations(path,visitor);
    }
  );
}


void Tree::visitType(const Path &path,const ItemVisitor &visitor)
{
  visitWrapper(
    path,
    [&](const Wrapper &wrapper){
      wrapper.visitType(visitor);
    }
  );
}


int Tree::findNChildren(const Path &path)
{
  int result = 0;

  visitWrapper(
    path,
    [&](const Wrapper &wrapper){
      result = wrapper.nChildren();
    }
  );

  return result;
}


void
  Tree::comboBoxItemIndexChanged(
    const Path &path,
    int index,
    OperationHandler &operation_handler
  )
{
  visitWrapper(
    path,
    [&](const Wrapper &wrapper){
      wrapper.comboBoxItemIndexChanged(path,index,operation_handler);
    }
  );
}


Wrapper &Tree::world()
{
  assert(_world_ptr);
  return *_world_ptr;
}
