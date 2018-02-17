#ifndef WRAPPER_HPP_
#define WRAPPER_HPP_

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
struct VoidWrapper;
struct NumericWrapper;
struct StringWrapper;
struct EnumerationWrapper;

using WrapperVisitor = std::function<void(const Wrapper &)>;


struct Wrapper {
  using OperationName = std::string;

  struct OperationHandler {
    virtual void addItem(const TreePath &) = 0;
    virtual void replaceTreeItems(const TreePath &) = 0;
    virtual void changeEnumerationValues(const TreePath &) const = 0;
  };

  struct Visitor {
    virtual void operator()(const VoidWrapper &) const = 0;
    virtual void operator()(const NumericWrapper &) const = 0;
    virtual void operator()(const EnumerationWrapper &) const = 0;
    virtual void operator()(const StringWrapper &) const = 0;
  };

  virtual std::vector<OperationName> operationNames() const = 0;

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      OperationHandler &handler
    ) const = 0;

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

  int nChildren(const TreePath &path) const
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

  Diagram *diagramPtr(const TreePath &path);

  virtual void accept(const Visitor &) const = 0;

  virtual std::string label() const = 0;
};


struct VoidWrapper : Wrapper {
  void accept(const Visitor &visitor) const override
  {
    visitor(*this);
  }
};


struct NumericWrapper : Wrapper {
  void accept(const Visitor &visitor) const override
  {
    visitor(*this);
  }

  virtual void setValue(int) const = 0;
};


struct StringWrapper : Wrapper {
  void accept(const Visitor &visitor) const override
  {
    visitor(*this);
  }

  virtual std::string value() const = 0;
};


struct EnumerationWrapper : Wrapper {
  void accept(const Visitor &visitor) const override
  {
    visitor(*this);
  }

  virtual std::vector<std::string> enumerationNames() const = 0;

  virtual void
    comboBoxItemIndexChanged(
      const TreePath &path,
      int index,
      OperationHandler &operation_handler
    ) const = 0;
};


using TreeOperationHandler = Wrapper::OperationHandler;


inline TreePath join(TreePath path,TreeItemIndex child_index)
{
  path.push_back(child_index);
  return path;
}

#endif /* WRAPPER_HPP_ */
