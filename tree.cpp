#include "tree.hpp"

#include <iostream>


using std::string;
using std::cerr;


Tree::Tree()
: _root_node(ItemType::root)
{
}


auto Tree::createCharmapperItem() -> Path
{
  return createItem({},ItemType::charmapper);
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


TreeItem::TreeItem(Type type_arg)
  : type(type_arg)
{
  if (type==Type::local_position) {
    // NodeIndex node_index =
    cerr << "Adding local position nodes\n";
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


#if 0
void TreeItem::addItem(const TreeItem &arg)
{
  child_items.push_back(arg);
}
#endif


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
  return getItem(path).type==ItemType::charmapper;
}


bool Tree::isMotionPassItem(const Path &path) const
{
  return getItem(path).type==ItemType::motion_pass;
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
