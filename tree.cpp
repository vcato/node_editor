#include "tree.hpp"


using std::string;


Tree::Tree()
: _root_node(NodeType::root)
{
}


auto Tree::createCharmapperItem() -> Path
{
  return createItem({},NodeType::charmapper);
}


auto Tree::createMotionPassItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::motion_pass);
}


auto Tree::createPosExprItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::pos_expr);
}


auto Tree::createTargetBodyItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::target_body);
}


auto Tree::createLocalPositionItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::local_position);
}


auto Tree::createGlobalPositionItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::global_position);
}


auto Tree::createWeightItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::weight);
}


auto Tree::createXItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::x);
}


auto Tree::createYItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::y);
}


auto Tree::createZItem(const Path &parent_path) -> Path
{
  return createItem(parent_path,NodeType::z);
}


auto Tree::createItem(const Path &parent_path,NodeType type) -> Path
{
  return join(parent_path,getNode(parent_path).createItem(type));
}


auto Tree::Node::createItem(NodeType type) -> Index
{
  Index result = child_nodes.size();
  child_nodes.push_back(Node(type));
  return result;
}


auto Tree::Node::getNode(const Path &path,int depth) const -> const Node &
{
  int path_length = path.size();

  if (depth==path_length) {
    return *this;
  }

  return child_nodes[path[depth]].getNode(path,depth+1);
}


bool Tree::isCharmapperItem(const Path &path) const
{
  return getNode(path).type==NodeType::charmapper;
}


bool Tree::isMotionPassItem(const Path &path) const
{
  return getNode(path).type==NodeType::motion_pass;
}


auto Tree::getNode(const Path &path) -> Node&
{
  const Tree &const_self = *this;
  const Node &const_node = const_self.getNode(path);
  return const_cast<Node&>(const_node);
}


auto Tree::getNode(const Path &path) const -> const Node &
{
  return _root_node.getNode(path,0);
}
