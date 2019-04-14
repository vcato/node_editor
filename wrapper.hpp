#ifndef WRAPPER_HPP_
#define WRAPPER_HPP_

#include <string>
#include <vector>
#include <cassert>
#include <functional>
#include <limits>
#include "treepath.hpp"
#include "wrapperstate.hpp"
#include "optional.hpp"
#include "observeddiagram.hpp"


class Diagram;
class DiagramEvaluationState;
struct Wrapper;
struct VoidWrapper;
struct NumericWrapper;
struct StringWrapper;
struct EnumerationWrapper;

using WrapperVisitor = std::function<void(const Wrapper &)>;

struct Wrapper {
  using OperationName = std::string;
  using Label = std::string;
  using Tag = WrapperState::Tag;

  struct TreeObserver {
    virtual void itemAdded(const TreePath &) = 0;
    virtual void itemReplaced(const TreePath &) = 0;
    virtual void itemRemoved(const TreePath &) = 0;
    virtual void itemLabelChanged(const TreePath &) = 0;
    virtual void itemValueChanged(const TreePath &) = 0;
  };

  struct SubclassVisitor {
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
  virtual bool canEditDiagram() const;
  virtual DiagramObserverPtr makeDiagramObserver() const { assert(false); }
  virtual const Diagram &defaultDiagram() const { assert(false); }

  virtual void accept(const SubclassVisitor &) const = 0;

  virtual Label label() const = 0;

  virtual Tag tag() const;

  virtual void setState(const WrapperState &) const = 0;
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

  if (depth == path_length) {
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
  void accept(const SubclassVisitor &visitor) const override
  {
    visitor(*this);
  }
};


template <>
struct LeafWrapper<VoidWrapper> : VoidWrapper {
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

  void setState(const WrapperState &) const override
  {
    assert(false);
  }
};


struct NumericWrapper : Wrapper {
  using Value = NumericValue;

  void accept(const SubclassVisitor &visitor) const override
  {
    visitor(*this);
  }

  // We'll need to pass the path and tree observer here in case changing
  // this value affects other parts of the tree.
  virtual void
    setValue(
      Value,
      const TreePath &path,
      TreeObserver &
    ) const = 0;

  virtual Value value() const = 0;

  static Value noMinimumValue() { return noNumericMinimum(); }

  static Value noMaximumValue() { return noNumericMaximum(); }

  virtual Value minimumValue() const
  {
    return noMinimumValue();
  }

  virtual Value maximumValue() const
  {
    return noMaximumValue();
  }
};


extern void
  visitNumericSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    const std::function<void(const NumericWrapper &)> &f
  );


struct StringWrapper : Wrapper {
  using Value = StringValue;

  void accept(const SubclassVisitor &visitor) const override
  {
    visitor(*this);
  }

  virtual Value value() const = 0;

  virtual void
    setValue(
      const Value &,
      const TreePath &path,
      TreeObserver &
    ) const = 0;
};


extern void
  visitStringSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    const std::function<void(const StringWrapper &)> &f
  );


struct EnumerationWrapper : Wrapper {
  using Index = int;
  using Value = Index;

  void accept(const SubclassVisitor &visitor) const override
  {
    visitor(*this);
  }

  virtual std::vector<std::string> enumerationNames() const = 0;

  virtual void
    setValue(
      Index,
      const TreePath &path,
      TreeObserver &
    ) const = 0;

  virtual Index value() const = 0;
};


using TreeObserver = Wrapper::TreeObserver;


extern WrapperState stateOf(const Wrapper &wrapper);
extern std::string makeTag(std::string label);


#endif /* WRAPPER_HPP_ */
