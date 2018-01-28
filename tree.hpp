#ifndef TREE_HPP_
#define TREE_HPP_

#include <string>

#include <vector>
#include "diagram.hpp"


struct TreeItem {
  using Path = std::vector<int>;
  using Index = int;

  enum class Type {
    root,
    charmapper,
    scene,
    motion_pass,
    pos_expr,
    target_body,
    source_body,
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
    using SizeType = int;

    struct OperationHandler {
      virtual void addCharmapper() = 0;
      virtual void addScene() = 0;
      virtual void addMotionPass(const Path &) = 0;
      virtual void addPosExpr(const Path &) = 0;
    };


    Tree();
    Path createCharmapperItem();
    Path createSceneItem();
    Path createMotionPassItem(const Path &);
    Path createPosExprItem(const Path &);
    Path createTargetBodyItem(const Path &);
    Path createSourceBodyItem(const Path &);
    Path createLocalPositionItem(const Path &);
    Path createGlobalPositionItem(const Path &);
    Path createWeightItem(const Path &);
    Path createXItem(const Path &);
    Path createYItem(const Path &);
    Path createZItem(const Path &);
    SizeType nChildItems(const Path &) const;
    void removeChildItems(const Path &);
    bool isCharmapperItem(const Path &path) const;
    bool isMotionPassItem(const Path &path) const;
    bool isSceneItem(const Path &path) const;
    bool isGlobalPositionItem(const Path &path) const;
    Diagram &itemDiagram(const Path &);

    void createXYZChildren(TreeItem &parent_item);

    using PerformOperationFunction =
      std::function<void (OperationHandler &)>;

    using OperationName = const std::string;
    using OperationVisitor =
      std::function<void(const OperationName &,PerformOperationFunction)>;

    void visitOperations(const Path &,OperationVisitor visitor);

  private:
    using Item = TreeItem;

    using ItemType = Item::Type;

    Item &getItem(const Path &);
    const Item &getItem(const Path &) const;
    Path createItem(const Path &parent_path,Item::Type type);
    ItemType itemType(const Path &) const;

    Item _root_node;
};

using TreeOperationHandler = Tree::OperationHandler;


inline Tree::Path join(Tree::Path path,Tree::Index child_index)
{
  path.push_back(child_index);
  return path;
}

#endif /* TREE_HPP_ */
