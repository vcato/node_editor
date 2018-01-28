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
    body,
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

  Index createItem(const TreeItem &item);
  TreeItem& createItem2(Type type);
};


struct WorldInterface {
  virtual void addScene() = 0;
};


class Tree {
  public:
    struct OperationHandler;

    using Path = std::vector<int>;
    using Index = int;
    using SizeType = int;
    using Item = TreeItem;
    using PerformOperationFunction =
      std::function<void (OperationHandler &)>;
    using OperationName = const std::string;
    using OperationVisitor =
      std::function<void(const OperationName &,PerformOperationFunction)>;

    struct OperationHandler {
      virtual void addItem(const Path &,const TreeItem &) = 0;
      virtual void
        replaceTreeItems(const Path &path,const TreeItem &items) = 0;
    };

    struct ItemVisitor {
      virtual void
        voidItem(const std::string &label) const = 0;

      virtual void
        numericItem(
          const std::string &label
        ) const = 0;

      virtual void
        enumeratedItem(
          const std::string &label,
          const std::vector<std::string> &enumeration_names
        ) const = 0;
    };

    Tree();

    void setWorldPtr(WorldInterface *arg) { _world_ptr = arg; }
    Path createItem(const Path &parent_path,const Item &);
    void comboBoxItemIndexChanged(const Path &,int index,OperationHandler &);
    SizeType nChildItems(const Path &) const;
    void removeChildItems(const Path &);
    Diagram &itemDiagram(const Path &);
    void visitOperations(const Path &,OperationVisitor visitor);
    void visitItem(const Item &,const ItemVisitor &visitor);

  private:
    using ItemType = Item::Type;

    Item &getItem(const Path &);
    const Item &getItem(const Path &) const;
    ItemType itemType(const Path &) const;

    WorldInterface *_world_ptr = nullptr;
    Item _root_node;
};

using TreeOperationHandler = Tree::OperationHandler;


inline Tree::Path join(Tree::Path path,Tree::Index child_index)
{
  path.push_back(child_index);
  return path;
}

#endif /* TREE_HPP_ */
