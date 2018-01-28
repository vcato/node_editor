#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include "defaultdiagrams.hpp"


using std::string;
using std::cerr;
using std::function;
using std::vector;


static void createXYZChildren(TreeItem &parent_item)
{
  using ItemType = TreeItem::Type;

  parent_item.createItem2(ItemType::x);
  parent_item.createItem2(ItemType::y);
  parent_item.createItem2(ItemType::z);
}


static TreeItem posExprItem()
{
  using ItemType = TreeItem::Type;
  TreeItem pos_expr_item(ItemType::pos_expr);
  pos_expr_item.createItem2(ItemType::target_body);
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(ItemType::local_position);
    createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(ItemType::global_position);
    createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }

  return pos_expr_item;
}


static TreeItem motionPassItem()
{
  using ItemType = TreeItem::Type;
  TreeItem motion_pass_item(ItemType::motion_pass);
  return motion_pass_item;
}


static TreeItem sceneItem()
{
  using ItemType = TreeItem::Type;
  TreeItem motion_pass_item(ItemType::scene);
  return motion_pass_item;
}


static TreeItem charmapperItem()
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


static TreeItem globalPositionComponentsItems()
{
  TreeItem items(TreeItem::Type::root);
  createXYZChildren(items);
  items.diagram = fromComponentsDiagram();
  return items;
}


static TreeItem globalPositionFromBodyItems()
{
  TreeItem items(TreeItem::Type::root);
  items.createItem(TreeItem::Type::source_body);
  TreeItem &local_position_item =
    items.createItem2(TreeItem::Type::local_position);
  items.diagram = fromBodyDiagram();
  createXYZChildren(local_position_item);
  return items;
}


void
  Tree::comboBoxItemIndexChanged(
    const Path &path,
    int index,
    OperationHandler &operation_handler
  )
{
  if (itemType(path)==TreeItem::Type::global_position) {
    switch (index) {
      case 0:
        // Components
        {
          TreeItem items = globalPositionComponentsItems();
          operation_handler.replaceTreeItems(path,items);
        }
        break;
      case 1:
        // From Body
        {
          TreeItem items = globalPositionFromBodyItems();
          operation_handler.replaceTreeItems(path,items);
        }
        break;
      default:
        assert(false);
    }
  }
}


void Tree::visitItem(const Item &item,const ItemVisitor &visitor)
{
  switch (item.type) {
    case TreeItem::Type::x:
      visitor.numericItem("X");
      break;
    case TreeItem::Type::y:
      visitor.numericItem("Y");
      break;
    case TreeItem::Type::z:
      visitor.numericItem("Z");
      break;
    case TreeItem::Type::global_position:
      {
        vector<string> enumeration_names = {"Components","From Body"};
        visitor.enumeratedItem("Global Position",enumeration_names);
      }
      break;
    case TreeItem::Type::local_position:
      visitor.voidItem("Local Position");
      break;
    case TreeItem::Type::target_body:
      {
        vector<string> enumeration_names = {"Body1","Body2","Body3"};
        visitor.enumeratedItem("Target Body",enumeration_names);
      }
      break;
    case TreeItem::Type::source_body:
      {
        vector<string> enumeration_names = {"Body1","Body2","Body3"};
        visitor.enumeratedItem("Source Body",enumeration_names);
      }
      break;
    case TreeItem::Type::pos_expr:
      visitor.voidItem("Pos Expr");
      break;
    case TreeItem::Type::motion_pass:
      visitor.voidItem("Motion Pass");
      break;
    case TreeItem::Type::scene:
      visitor.voidItem("Scene");
      break;
    case TreeItem::Type::charmapper:
      visitor.voidItem("Charmapper");
      break;
    default:
      assert(false);
  }
}
