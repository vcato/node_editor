#ifndef TREE_HPP_
#define TREE_HPP_

#include <string>

#include <vector>
#include <cassert>
#include "diagram.hpp"


using TreeItemIndex = int;
using TreePath = std::vector<TreeItemIndex>;


inline TreePath parentPath(const TreePath &path)
{
  TreePath result = path;
  result.pop_back();
  return result;
}


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
    virtual void voidItem() const = 0;

    virtual void numericItem() const = 0;

    virtual void
      enumeratedItem(
        const std::vector<std::string> &enumeration_names
      ) const = 0;

    virtual void stringItem() const = 0;
  };

  virtual void
    visitOperations(
      const TreePath &,
      const OperationVisitor &
    ) const = 0;

  void visitOperations(const Path &path,const OperationVisitor &visitor)
  {
    visitWrapper(
      path,
      [&](const Wrapper &wrapper){
	wrapper.visitOperations(path,visitor);
      }
    );
  }

  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const = 0;

  void
    visitWrapper(
      const TreePath &path,
      const WrapperVisitor &visitor
    ) const
  {
    visitWrapper(path,0,visitor);
  }

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

  int nChildren(const Path &path) const
  {
    int result = 0;

    visitWrapper(
      path,
      [&](const Wrapper &wrapper){
	result = wrapper.nChildren();
      }
    );

    return result;
  }

  virtual int nChildren() const = 0;

  virtual Diagram *diagramPtr() const = 0;

  Diagram *diagramPtr(const Path &path);

  virtual void
    comboBoxItemIndexChanged(
      const TreePath &path,
      int index,
      OperationHandler &operation_handler
    ) const = 0;

  virtual void setValue(int) const = 0;

  void
    comboBoxItemIndexChanged(
      const Path &path,
      int index,
      OperationHandler &operation_handler
    )
  {
    visitWrapper(
      path,
      [&](const Wrapper &wrapper){
	wrapper.comboBoxItemIndexChanged(path,index,operation_handler);
      }
    );
  }


  virtual void visitType(const TypeVisitor &) const = 0;

  virtual std::string label() const = 0;
};


struct VoidWrapper : Wrapper {
  virtual void
    comboBoxItemIndexChanged(
      const TreePath &,
      int /*index*/,
      OperationHandler &
    ) const
  {
    assert(false);
  }

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.voidItem();
  }

  void setValue(int) const override
  {
    assert(false);
  }
};


struct NumericWrapper : Wrapper {
  void
    comboBoxItemIndexChanged(
      const TreePath &,
      int /*index*/,
      OperationHandler &
    ) const override
  {
    assert(false);
  }

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.numericItem();
  }
};


struct StringWrapper : Wrapper {
  void
    comboBoxItemIndexChanged(
      const TreePath &,
      int /*index*/,
      OperationHandler &
    ) const override
  {
    assert(false);
  }

  void setValue(int) const override
  {
    assert(false);
  }

  virtual void visitType(const TypeVisitor &visitor) const
  {
    visitor.stringItem();
  }
};


struct EnumerationWrapper : Wrapper {
  void setValue(int) const override
  {
    assert(false);
  }
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


using TreeOperationHandler = Wrapper::OperationHandler;


inline TreePath join(TreePath path,TreeItemIndex child_index)
{
  path.push_back(child_index);
  return path;
}

#endif /* TREE_HPP_ */
