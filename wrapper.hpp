#ifndef WRAPPER_HPP_
#define WRAPPER_HPP_

#include <string>

#include <vector>
#include <cassert>
#include "diagram.hpp"
#include "treepath.hpp"


struct Wrapper;
struct VoidWrapper;
struct NumericWrapper;
struct StringWrapper;
struct EnumerationWrapper;

using WrapperVisitor = std::function<void(const Wrapper &)>;


struct Wrapper {
  using OperationName = std::string;
  using Label = std::string;

  struct TreeObserver {
    virtual void itemAdded(const TreePath &) = 0;
    virtual void itemReplaced(const TreePath &) = 0;
    virtual void itemRemoved(const TreePath &) = 0;
    virtual void enumarationValuesChanged(const TreePath &) const = 0;
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
      TreeObserver &
    ) const = 0;

  virtual int nChildren() const = 0;

  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const = 0;

  virtual Diagram *diagramPtr() const { return nullptr; }

  virtual void diagramChanged() const {}

  virtual void accept(const Visitor &) const = 0;

  virtual Label label() const = 0;
};


template <typename T>
struct LeafWrapper : T {
  void
    withChildWrapper(
      int /*child_index*/,
      const WrapperVisitor &
    ) const override
  {
    assert(false);
  }

  int nChildren() const override
  {
    return 0;
  }
};


template <typename T>
struct NoOperationWrapper : T {
  std::vector<std::string> operationNames() const override
  {
    return {};
  }

  void
    executeOperation(
      int /*operation_index*/,
      const TreePath &,
      Wrapper::TreeObserver &
    ) const override
  {
    assert(false);
  }
};


inline void
  visitSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    const WrapperVisitor &visitor,
    int depth = 0
  )
{
  int path_length = path.size();

  if (depth==path_length) {
    visitor(wrapper);
    return;
  }

  int child_index = path[depth];

  wrapper.withChildWrapper(
    child_index,
    [&](const Wrapper &child_wrapper){
      visitSubWrapper(child_wrapper,path,visitor,depth+1);
    }
  );
}


extern void
  visitEnumerationSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    std::function<void(const EnumerationWrapper &)> f
  );


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


extern void
  visitNumericSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    const std::function<void(const NumericWrapper &)> &f
  );

struct StringWrapper : Wrapper {
  void accept(const Visitor &visitor) const override
  {
    visitor(*this);
  }

  virtual std::string value() const = 0;

  virtual void setValue(const std::string &) const = 0;
};


extern void
  visitStringSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    const std::function<void(const StringWrapper &)> &f
  );


struct EnumerationWrapper : Wrapper {
  void accept(const Visitor &visitor) const override
  {
    visitor(*this);
  }

  virtual std::vector<std::string> enumerationNames() const = 0;

  virtual void
    setValue(
      const TreePath &path,
      int index,
      TreeObserver &
    ) const = 0;
};


using TreeObserver = Wrapper::TreeObserver;


inline TreePath join(TreePath path,TreeItemIndex child_index)
{
  path.push_back(child_index);
  return path;
}

#endif /* WRAPPER_HPP_ */
