#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include "defaultdiagrams.hpp"
#include "worldpolicies.hpp"


using std::string;
using std::cerr;
using std::function;
using std::vector;
using world_policies::RootPolicy;

using Path = Tree::Path;
using OperationVisitor = Tree::OperationVisitor;


Tree::Tree()
: _root_item(RootPolicy(*this))
{
}


Path Tree::createItem(const Path &parent_path,const Item &item)
{
  return join(parent_path,getItem(parent_path).createItem(item));
}


TreeItem::TreeItem(Policy policy_arg)
  : diagram(policy_arg.defaultDiagram()),
    policy(policy_arg)
{
}


void TreeItem::visit(const Visitor &visitor) const
{
  policy.visitType(visitor);
}


auto TreeItem::createItem(const TreeItem &item) -> Index
{
  Index result = child_items.size();
  child_items.push_back(TreeItem(item.policy));
  return result;
}


auto TreeItem::createItem2(TreeItem::Policy policy) -> TreeItem&
{
  child_items.push_back(TreeItem(policy));
  return child_items.back();
}


auto TreeItem::createItem2(const TreeItem &item) -> TreeItem&
{
  child_items.push_back(TreeItem(item.policy));
  return child_items.back();
}


auto TreeItem::getItem(const Path &path,int depth) const -> const TreeItem &
{
  int path_length = path.size();

  if (depth==path_length) {
    return *this;
  }

  return child_items[path[depth]].getItem(path,depth+1);
}


auto Tree::getItem(const Path &path) -> Item&
{
  const Tree &const_self = *this;
  const Item &const_node = const_self.getItem(path);
  return const_cast<Item&>(const_node);
}


auto Tree::getItem(const Path &path) const -> const Item &
{
  return _root_item.getItem(path,0);
}


Diagram *Tree::itemDiagramPtr(const Path &path)
{
  return &getItem(path).diagram;
}


void Tree::setItemDiagram(const Path &path,const Diagram &new_diagram)
{
  getItem(path).diagram = new_diagram;
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
  if (world().visitOperations(path,/*depth*/0,visitor)) {
    return;
  }

  getItem(path).visitOperations(path,visitor);
}


void
  Tree::comboBoxItemIndexChanged(
    const Path &path,
    int index,
    OperationHandler &operation_handler
  )
{
  getItem(path).comboBoxItemIndexChanged(path,index,operation_handler);
}


void Tree::visitItem(const Item &item,const ItemVisitor &visitor)
{
  item.visit(visitor);
}


WorldInterface &Tree::world()
{
  assert(_world_ptr);
  return *_world_ptr;
}
