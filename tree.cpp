#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>


using std::string;
using std::cerr;
using std::function;


Tree::Tree()
: _root_node(ItemType::root)
{
}


auto Tree::createCharmapperItem() -> Path
{
  return createItem({},ItemType::charmapper);
}


auto Tree::createSceneItem() -> Path
{
  return createItem({},ItemType::scene);
}


auto Tree::createMotionPassItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::motion_pass);
}


auto Tree::createPosExprItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::pos_expr);
}


auto Tree::createTargetBodyItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::target_body);
}


auto Tree::createSourceBodyItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::source_body);
}


auto Tree::createLocalPositionItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::local_position);
}


auto Tree::createGlobalPositionItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::global_position);
}


auto Tree::createWeightItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::weight);
}


auto Tree::createXItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::x);
}


auto Tree::createYItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::y);
}


auto Tree::createZItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,ItemType::z);
}


auto Tree::createItem(const Path &parent_path,ItemType type) -> Path
{
  return join(parent_path,getItem(parent_path).createItem(type));
}


auto Tree::itemType(const Path &path) const -> ItemType
{
  return getItem(path).type;
}


TreeItem::TreeItem(Type type_arg)
  : type(type_arg)
{
  if (type==Type::local_position) {
    NodeIndex vector_index = diagram.addNode("[$,$,$]");
    diagram.node(vector_index).setPosition({100,180});

    NodeIndex x_index = diagram.addNode("x");
    diagram.connectNodes(x_index,0,vector_index,0);
    diagram.node(x_index).setPosition({20,200});

    NodeIndex y_index = diagram.addNode("y");
    diagram.node(y_index).setPosition({20,150});
    diagram.connectNodes(y_index,0,vector_index,1);

    NodeIndex z_index = diagram.addNode("z");
    diagram.node(z_index).setPosition({20,100});
    diagram.connectNodes(z_index,0,vector_index,2);

    NodeIndex local_postion_index = diagram.addNode("local_position=$");
    diagram.node(local_postion_index).setPosition({230,150});

    diagram.connectNodes(vector_index,0,local_postion_index,0);
  }
}


auto TreeItem::createItem(Type type) -> Index
{
  Index result = child_items.size();
  child_items.push_back(TreeItem(type));
  return result;
}


auto TreeItem::createItem2(Type type) -> TreeItem&
{
  child_items.push_back(TreeItem(type));
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


bool Tree::isCharmapperItem(const Path &path) const
{
  return itemType(path)==ItemType::charmapper;
}


bool Tree::isMotionPassItem(const Path &path) const
{
  return itemType(path)==ItemType::motion_pass;
}


bool Tree::isSceneItem(const Path &path) const
{
  return itemType(path)==ItemType::scene;
}


bool Tree::isGlobalPositionItem(const Path &path) const
{
  return itemType(path)==ItemType::global_position;
}


auto Tree::getItem(const Path &path) -> Item&
{
  const Tree &const_self = *this;
  const Item &const_node = const_self.getItem(path);
  return const_cast<Item&>(const_node);
}


auto Tree::getItem(const Path &path) const -> const Item &
{
  return _root_node.getItem(path,0);
}


Diagram& Tree::itemDiagram(const Path &path)
{
  return getItem(path).diagram;
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


void Tree::createXYZChildren(TreeItem &parent_item)
{
  using ItemType = TreeItem::Type;

  parent_item.createItem2(ItemType::x);
  parent_item.createItem2(ItemType::y);
  parent_item.createItem2(ItemType::z);
}


void
  Tree::visitOperations(
    function<
      void(
        const string &,
        function<void (TreeOperationHandler &)> perform_function
      )> visitor
  )
{
  visitor(
    "Add Charmapper",
    [](TreeOperationHandler &handler){
      handler.addCharmapper();
    }
  );
  visitor(
    "Add Scene",
    [](TreeOperationHandler &handler){
      handler.addScene();
    }
  );
}
