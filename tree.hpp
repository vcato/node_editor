#include <string>

#include <vector>


class Tree {
  public:
    using Path = std::vector<int>;
    using Index = int;

    Tree();
    Path createCharmapperItem();
    Path createMotionPassItem(const Path &);
    Path createPosExprItem(const Path &);
    Path createTargetBodyItem(const Path &);
    Path createLocalPositionItem(const Path &);
    Path createGlobalPositionItem(const Path &);
    Path createWeightItem(const Path &);
    Path createXItem(const Path &);
    Path createYItem(const Path &);
    Path createZItem(const Path &);
    bool isCharmapperItem(const Path &path) const;
    bool isMotionPassItem(const Path &path) const;

  private:
    struct Node {
      enum class Type {
        root,
        charmapper,
        motion_pass,
        pos_expr,
        target_body,
        local_position,
        global_position,
        weight,
        x,
        y,
        z
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

    using NodeType = Node::Type;

    Node &getNode(const Path &);
    const Node &getNode(const Path &) const;
    Path createItem(const Path &parent_path,Node::Type type);

    Node _root_node;
};


inline Tree::Path join(Tree::Path path,Tree::Index child_index)
{
  path.push_back(child_index);
  return path;
}
