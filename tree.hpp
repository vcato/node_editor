#ifndef TREE_HPP_
#define TREE_HPP_

#include <string>

#include <vector>
#include <cassert>
#include "diagram.hpp"


using TreePath = std::vector<int>;


struct Wrapper;

using WrapperVisitor = std::function<void(const Wrapper &)>;


struct Wrapper {
  using OperationName = const std::string;
  using Path = TreePath;

  struct OperationHandler {
    virtual void addItem(const Path &) = 0;
    virtual void replaceTreeItems(const Path &path) = 0;
  };

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

  virtual void
    visitOperations(
      const TreePath &,
      const OperationVisitor &
    ) const = 0;


  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const = 0;

  void
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

    int child_index = path[depth];

    withChildWrapper(
      child_index,
      [&](const Wrapper &wrapper){
        wrapper.visitWrapper(path,depth+1,visitor);
      }
    );
  }

  virtual int nChildren() const = 0;

  virtual Diagram *diagramPtr() const = 0;

  virtual void
    comboBoxItemIndexChanged(
      const TreePath &path,
      int index,
      OperationHandler &operation_handler
    ) const = 0;

  virtual void visitType(const TypeVisitor &) const = 0;
};


struct SimpleWrapper : Wrapper {
  virtual void
    comboBoxItemIndexChanged(
      const TreePath &/*path*/,
      int /*index*/,
      OperationHandler &/*operation_handler*/
    ) const
  {
    assert(false);
  }
};


class Tree {
  public:
    struct Item {
      using Index = int;

      std::vector<Item> child_items;

      Index createItem();
    };

    using Path = TreePath;
    using Index = Item::Index;
    using SizeType = int;
    using ItemVisitor = Wrapper::TypeVisitor;
    using OperationHandler = Wrapper::OperationHandler;
    using OperationVisitor = Wrapper::OperationVisitor;

    Tree();

    void setWorldPtr(Wrapper *arg) { _world_ptr = arg; }
    Path createItem(const Path &parent_path);
    void comboBoxItemIndexChanged(const Path &,int index,OperationHandler &);
    SizeType nChildItems(const Path &) const;
    void removeChildItems(const Path &);
    Diagram *itemDiagramPtr(const Path &);
    void visitOperations(const Path &,const OperationVisitor &visitor);
    void visitType(const Path &,const ItemVisitor &);
    int findNChildren(const Path &);
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
