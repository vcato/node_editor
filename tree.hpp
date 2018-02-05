#ifndef TREE_HPP_
#define TREE_HPP_

#include <string>

#include <vector>
#include <cassert>
#include "diagram.hpp"


using TreePath = std::vector<int>;


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

  struct TypeVisitor {
    virtual void voidItem(const std::string &label) const = 0;

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
      virtual void visitType(const TypeVisitor &) const = 0;
    };

    template <typename T>
    struct BasicPolicy : PolicyInterface {
      T object;

      BasicPolicy(const T& arg) : object(arg) {}

      virtual void visitType(const TypeVisitor &visitor) const
      {
        object.visitType(visitor);
      }

      virtual PolicyInterface *clone() const
      {
        return new BasicPolicy<T>(*this);
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
        const TypeVisitor &visitor
      ) const
    {
      interface().visitType(visitor);
    }

    ~Policy() { delete ptr; }
  };

  std::vector<TreeItem> child_items;
  Policy policy;

  TreeItem(Policy);

  const TreeItem &getItem(const Path &,int depth) const;

  Index createItem(const TreeItem::Policy &);
  TreeItem& createItem2(Policy);
};


struct Wrapper;

using WrapperVisitor = std::function<void(const Wrapper &)>;


struct Wrapper {
  virtual void
    visitOperations(
      const TreePath &,
      const TreeItem::OperationVisitor &
    ) const = 0;


  virtual void
    visitWrapper(
      const TreePath &path,
      int depth,
      const WrapperVisitor &visitor
    ) const
  {
    int path_length = path.size();

    if (depth==path_length) {
      visitor(*this);

      return;
    }

    visitChildWrapper(path,depth,visitor);
  }

  virtual void
    visitChildWrapper(
      const TreePath &path,
      int depth,
      const WrapperVisitor &visitor
    ) const = 0;

  virtual Diagram *diagramPtr() const = 0;

  virtual void
    comboBoxItemIndexChanged(
      const TreePath &path,
      int index,
      TreeItem::OperationHandler &operation_handler
    ) const = 0;

  virtual void visitType(const TreeItem::TypeVisitor &) const = 0;
};


struct SimpleWrapper : Wrapper {
  virtual void
    comboBoxItemIndexChanged(
      const TreePath &/*path*/,
      int /*index*/,
      TreeItem::OperationHandler &/*operation_handler*/
    ) const
  {
    assert(false);
  }
};


class Tree {
  public:
    using Path = TreePath;
    using Index = int;
    using SizeType = int;
    using Item = TreeItem;
    using ItemVisitor = Item::TypeVisitor;
    using OperationHandler = Item::OperationHandler;
    using OperationVisitor = Item::OperationVisitor;

    Tree();

    void setWorldPtr(Wrapper *arg) { _world_ptr = arg; }
    Path createItem(const Path &parent_path);
    void comboBoxItemIndexChanged(const Path &,int index,OperationHandler &);
    SizeType nChildItems(const Path &) const;
    void removeChildItems(const Path &);
    Diagram *itemDiagramPtr(const Path &);
    void visitOperations(const Path &,const OperationVisitor &visitor);
    void visitType(const Path &,const Item::TypeVisitor &);
    Wrapper &world();

  private:
    Item &getItem(const Path &);
    const Item &getItem(const Path &) const;
    void visitWrapper(const Path &,const WrapperVisitor &);

    Wrapper *_world_ptr = nullptr;
    Item _root_item;
};

using TreeOperationHandler = Tree::OperationHandler;


inline Tree::Path join(Tree::Path path,Tree::Index child_index)
{
  path.push_back(child_index);
  return path;
}

#endif /* TREE_HPP_ */
