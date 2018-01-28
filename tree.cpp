#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include "defaultdiagrams.hpp"


using std::string;
using std::cerr;
using std::function;


static TreeItem posExprItem(Tree &tree)
{
  using ItemType = TreeItem::Type;
  TreeItem pos_expr_item(ItemType::pos_expr);
  pos_expr_item.createItem2(ItemType::target_body);
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(ItemType::local_position);
    tree.createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(ItemType::global_position);
    tree.createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }

  return pos_expr_item;
}


TreeItem Tree::posExprItem()
{
  return ::posExprItem(*this);
}


TreeItem Tree::motionPassItem()
{
  using ItemType = TreeItem::Type;
  TreeItem motion_pass_item(ItemType::motion_pass);
  return motion_pass_item;
}


TreeItem Tree::sceneItem()
{
  using ItemType = TreeItem::Type;
  TreeItem motion_pass_item(ItemType::scene);
  return motion_pass_item;
}


TreeItem Tree::charmapperItem()
{
  using ItemType = TreeItem::Type;
  TreeItem motion_pass_item(ItemType::charmapper);
  return motion_pass_item;
}


Tree::Tree()
: _root_node(ItemType::root)
{
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


void Tree::visitOperations(const Path &path,OperationVisitor visitor)
{
  if (itemType(path)==ItemType::root) {
    visitor(
      "Add Charmapper",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,charmapperItem());
      }
    );
    visitor(
      "Add Scene",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,sceneItem());
      }
    );
  }
  else if (itemType(path)==ItemType::charmapper) {
    visitor(
      "Add Motion Pass",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,motionPassItem());
      }
    );
  }
  else if (itemType(path)==ItemType::motion_pass) {
    visitor(
      "Add Pos Expr",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,posExprItem());
      }
    );
  }
  else {
    cerr << "Tree::visitOperations: unknown item type\n";
  }
}
