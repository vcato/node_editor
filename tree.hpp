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

  struct OperationHandler;

  struct OperationHandler {
    virtual void addItem(const Path &,const TreeItem &) = 0;
    virtual void
      replaceTreeItems(const Path &path,const TreeItem &items) = 0;
  };

  using OperationName = const std::string;

  using PerformOperationFunction =
    std::function<void (OperationHandler &)>;

  using OperationVisitor =
    std::function<void(const OperationName &,PerformOperationFunction)>;

  struct Visitor {
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

  struct Policy {
    struct PolicyInterface {
      virtual ~PolicyInterface() {}
      virtual PolicyInterface *clone() = 0;
      virtual void visitOperations(const Path &,const OperationVisitor &) = 0;
    };

    template <typename T>
    struct BasicPolicy : PolicyInterface {
      T object;

      BasicPolicy(const T& arg) : object(arg) {}

      void visitOperations(const Path &path,const OperationVisitor &visitor)
      {
        object.visitOperations(path,visitor);
      }

      virtual PolicyInterface *clone()
      {
        return new BasicPolicy<T>(*this);
      }
    };

    PolicyInterface *ptr = 0;

    template <typename T>
    Policy(const T &arg)
    : ptr(new BasicPolicy<T>{arg})
    {
    }

    Policy(const Policy &arg)
    : ptr(arg.ptr->clone())
    {
    }

    void operator=(const Policy &) = delete;

    void visitOperations(const Path &path,const OperationVisitor &visitor);

    ~Policy()
    {
      delete ptr;
    }
  };

  Type type;
  Diagram diagram;
  std::vector<TreeItem> child_items;
  Policy policy;

  TreeItem(Type);
  TreeItem(Type,Policy);

  const TreeItem &getItem(const Path &,int depth) const;

  Index createItem(const TreeItem &item);
  TreeItem& createItem2(Type type);
  void visit(const Visitor &) const;
  void
    visitOperations(const Path &path,const OperationVisitor &visitor)
    {
      policy.visitOperations(path,visitor);
    }
};


struct WorldInterface {
  virtual void addScene() = 0;
};


class Tree {
  public:
    using Path = std::vector<int>;
    using Index = int;
    using SizeType = int;
    using Item = TreeItem;
    using ItemVisitor = Item::Visitor;
    using OperationHandler = Item::OperationHandler;
    using OperationVisitor = Item::OperationVisitor;

    Tree();

    void setWorldPtr(WorldInterface *arg) { _world_ptr = arg; }
    Path createItem(const Path &parent_path,const Item &);
    void comboBoxItemIndexChanged(const Path &,int index,OperationHandler &);
    SizeType nChildItems(const Path &) const;
    void removeChildItems(const Path &);
    Diagram &itemDiagram(const Path &);
    void visitOperations(const Path &,OperationVisitor visitor);
    void visitItem(const Item &,const ItemVisitor &visitor);
    WorldInterface &world();

  private:
    using ItemType = Item::Type;

    Item &getItem(const Path &);
    const Item &getItem(const Path &) const;
    ItemType itemType(const Path &) const;

    WorldInterface *_world_ptr = nullptr;
    Item _root_item;
};

using TreeOperationHandler = Tree::OperationHandler;


inline Tree::Path join(Tree::Path path,Tree::Index child_index)
{
  path.push_back(child_index);
  return path;
}

#endif /* TREE_HPP_ */
