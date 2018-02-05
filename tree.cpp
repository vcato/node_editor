#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include "defaultdiagrams.hpp"
#include "worldpolicies.hpp"
#include "streamvector.hpp"


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


Path Tree::createItem(const Path &parent_path,const Item::Policy &policy)
{
#if 1
  int index = getItem(parent_path).createItem(policy);
#else
  const Item &parent_item = getItem(parent_path);
  int index = -1;
  visitWrapper(
    parent_path,
    [&](const Wrapper &wrapper){
      index = parent_item.createItem(wrapper);
    }
  );
#endif
  return join(parent_path,index);
}


TreeItem::TreeItem(Policy policy_arg)
  : policy(policy_arg)
{
}


auto TreeItem::createItem(const TreeItem::Policy &policy) -> Index
{
  Index result = child_items.size();
  child_items.push_back(TreeItem(policy));
  return result;
}


auto TreeItem::createItem2(TreeItem::Policy policy) -> TreeItem&
{
  child_items.push_back(TreeItem(policy));
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
