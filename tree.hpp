#include <string>

#include <vector>


class Tree {
  public:
    using Path = std::vector<int>;
    using Index = int;

    Tree();
    Index createCharmapperItem();
    Path createCharmapperItem2();
    Index createMotionPassItem(const Path &);
    Index createPosExprItem(const Path &);
    bool isCharmapperItem(const Path &path) const;
    bool isMotionPassItem(const Path &path) const;

  private:
    struct Node {
      enum class Type {
        root,
        charmapper,
        motion_pass,
        pos_expr
      };

      Type type;
      std::vector<Node> child_nodes;

      Node(Type type_arg)
      : type(type_arg)
      {
      }

      const Node &getNode(const Path &,int depth) const;

      Index createItem(Type type);
    };

    Node &getNode(const Path &);
    const Node &getNode(const Path &) const;
    Index createItem(const Path &parent_path,Node::Type type);

    Node _root_node;
};


inline Tree::Path join(Tree::Path path,Tree::Index child_index)
{
  path.push_back(child_index);
  return path;
}
