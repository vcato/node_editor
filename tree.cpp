#include "tree.hpp"


using std::string;


Tree::Tree()
: _root_node(Node::Type::root)
{
}


auto Tree::createCharmapperItem() -> Index
{
  return _root_node.createItem(Node::Type::charmapper);
}


auto Tree::createMotionPassItem(const Path &parent_path) -> Index
{
  return createItem(parent_path,Node::Type::motion_pass);
}


auto Tree::createItem(const Path &parent_path,Node::Type type) -> Index
{
  return getNode(parent_path).createItem(type);
}


auto Tree::Node::createItem(Node::Type type) -> Index
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
  return getNode(path).type==Node::Type::charmapper;
}


bool Tree::isMotionPassItem(const Path &path) const
{
  return getNode(path).type==Node::Type::motion_pass;
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
