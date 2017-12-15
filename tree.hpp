#include <string>

#include <vector>


class Tree {
  public:
    using Path = std::vector<int>;
    using Index = int;

    Tree();
    Index createCharmapperItem();
    Index createMotionPassItem(const Path &);
    bool isCharmapperItem(const Path &path) const;
    bool isMotionPassItem(const Path &path) const;

  private:
    struct Node {
      enum class Type {
        root,
        charmapper,
        motion_pass
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