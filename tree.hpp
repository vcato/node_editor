#ifndef TREE_HPP_
#define TREE_HPP_

#include <string>

#include <vector>
#include <cassert>
#include "diagram.hpp"


struct TreeItem {
  using Path = std::vector<int>;
  using Index = int;

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
      virtual PolicyInterface *clone() const = 0;
      virtual void visitType(const Visitor &) const = 0;
      virtual void
        comboBoxItemIndexChanged(
          const Path &path,
          int index,
          OperationHandler &operation_handler
        ) = 0;
      virtual Diagram defaultDiagram() = 0;
    };

    template <typename T>
    struct BasicPolicy : PolicyInterface {
      T object;

      BasicPolicy(const T& arg) : object(arg) {}

      virtual void visitType(const Visitor &visitor) const
      {
        object.visitType(visitor);
      }

      virtual PolicyInterface *clone() const
      {
        return new BasicPolicy<T>(*this);
      }

      virtual void
        comboBoxItemIndexChanged(
          const Path &path,
          int index,
          OperationHandler &operation_handler
        )
      {
        object.comboBoxItemIndexChanged(path,index,operation_handler);
      }

      virtual Diagram defaultDiagram()
      {
        return object.defaultDiagram();
      }
    };

    PolicyInterface *ptr = 0;

    const PolicyInterface &interface() const
    {
      assert(ptr);
      return *ptr;
    }

    PolicyInterface &interface()
    {
      assert(ptr);
      return *ptr;
    }

    template <typename T>
    Policy(const T &arg)
    : ptr(new BasicPolicy<T>{arg})
    {
    }

    Policy(const Policy &arg)
    : ptr(arg.interface().clone())
    {
    }

    void operator=(const Policy &) = delete;

    void
      visitType(
        const Visitor &visitor
      ) const
    {
      interface().visitType(visitor);
    }

    Diagram defaultDiagram() { return interface().defaultDiagram(); }

    void
      comboBoxItemIndexChanged(
        const Path &path,
        int index,
        OperationHandler &operation_handler
      )
    {
      interface().comboBoxItemIndexChanged(path,index,operation_handler);
    }

    ~Policy() { delete ptr; }
  };

  Diagram diagram;
  std::vector<TreeItem> child_items;
  Policy policy;

  TreeItem(Policy);

  const TreeItem &getItem(const Path &,int depth) const;
  Diagram *diagramPtr() { return &diagram; }

  Index createItem(const TreeItem &item);
  TreeItem& createItem2(const TreeItem &);
  TreeItem& createItem2(Policy);
  void visit(const Visitor &) const;

  void
    comboBoxItemIndexChanged(
      const Path &path,
      int index,
      OperationHandler &operation_handler
    )
  {
    policy.comboBoxItemIndexChanged(path,index,operation_handler);
  }
};


using TreePath = std::vector<int>;

struct WorldInterface {
  using OperationVisitor = TreeItem::OperationVisitor;

  virtual bool
    visitOperations(const TreePath &,int depth,const OperationVisitor &) = 0;
};


class Tree {
  public:
    using Path = TreePath;
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
    Diagram *itemDiagramPtr(const Path &);
    void setItemDiagram(const Path &,const Diagram &);
    void visitOperations(const Path &,const OperationVisitor &visitor);
    void visitItem(const Item &,const ItemVisitor &visitor);
    WorldInterface &world();

  private:
    Item &getItem(const Path &);
    const Item &getItem(const Path &) const;

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
