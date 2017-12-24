#include <string>

#include <vector>
#include "diagram.hpp"


struct TreeItem {
  using Path = std::vector<int>;
  using Index = int;

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
  Diagram diagram;
  std::vector<TreeItem> child_items;

  TreeItem(Type);

  const TreeItem &getItem(const Path &,int depth) const;

  Index createItem(Type type);
  TreeItem& createItem2(Type type);
};


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
    // void replaceChildren(const Path &,const TreeItem &) {}
    bool isCharmapperItem(const Path &path) const;
    bool isMotionPassItem(const Path &path) const;
    Diagram &itemDiagram(const Path &);

  private:
    using Item = TreeItem;

    using ItemType = Item::Type;

    Item &getItem(const Path &);
    const Item &getItem(const Path &) const;
    Path createItem(const Path &parent_path,Item::Type type);

    Item _root_node;
};


inline Tree::Path join(Tree::Path path,Tree::Index child_index)
{
  path.push_back(child_index);
  return path;
}
