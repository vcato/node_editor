#include "wrapper.hpp"

#include <cassert>
#include <iostream>
#include "wrapperutil.hpp"
#include "diagram.hpp"

#define ADD_TEST 0

using std::string;
using std::vector;
using std::function;
using std::cerr;


namespace {
struct ZWrapper : NoOperationWrapper<LeafWrapper<VoidWrapper>> {
  virtual Label label() const { return "Z"; }
};
}


namespace {
struct YWrapper : NoOperationWrapper<LeafWrapper<VoidWrapper>> {
  virtual Label label() const { return "Y"; }

  virtual int nChildren() const { return 1; }

  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const
  {
    if (child_index==0) {
      return visitor(ZWrapper());
    }

    assert(false);
  }
};
}


namespace {
struct XWrapper : NoOperationWrapper<VoidWrapper> {
  virtual Label label() const { return "X"; }

  virtual int nChildren() const { return 1; }

  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const
  {
    if (child_index==0) {
      return visitor(YWrapper());
    }

    assert(false);
  }

  void setState(const WrapperState &) const override
  {
    assert(false);
  }
};
}


namespace {
struct TestWrapper : NoOperationWrapper<VoidWrapper> {
  Label label() const { return "test"; }

  virtual int nChildren() const
  {
    return 1;
  }

  virtual void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const
  {
    if (child_index==0) {
      visitor(XWrapper());
      return;
    }

    assert(false);
  }

  void setState(const WrapperState &) const override
  {
    assert(false);
  }
};
}


namespace {
struct TestWrapper2 : NoOperationWrapper<VoidWrapper> {
  vector<TestWrapper2> children;
  string label_member;
  Diagram *diagram_ptr = nullptr;

  void setDiagramPtr(Diagram *arg)
  {
    diagram_ptr = arg;
  }

  Diagram *diagramPtr() const override
  {
    return diagram_ptr;
  }

  int nChildren() const override
  {
    return children.size();
  }

  void
    withChildWrapper(
      int /*child_index*/,
      const WrapperVisitor &/*visitor*/
    ) const override
  {
    assert(false);
  }

  Label label() const override
  {
    return label_member;
  }

  void setState(const WrapperState &) const override
  {
    assert(false);
  }
};
}



static void testWithEmptyString()
{
  TestWrapper wrapper;
  TreePath result = makePath(wrapper,"");
  assert(result.empty());
}


static void testWithSingleComponent()
{
  TreePath result = makePath(TestWrapper(),"X");
  TreePath expected_result = {0};
  assert(result==expected_result);
}


static void testWithTwoComponents()
{
  TreePath result = makePath(TestWrapper(),"X|Y");
  TreePath expected_result = {0,0};
  assert(result==expected_result);
}


static void testWithThreeComponents()
{
  TreePath result = makePath(TestWrapper(),"X|Y|Z");
  TreePath expected_result = {0,0,0};
  assert(result==expected_result);
}


#if ADD_TEST
static void testWithDiagram()
{
  TestWrapper2 wrapper;
  Diagram diagram;
  wrapper.setDiagramPtr(&diagram);
  WrapperState state = stateOf(wrapper);
  printStateOn(cerr,state);
  assert(state.children.size()==1);
  assert(state.children[0].tag=="diagram");
}
#endif


int main()
{
  testWithEmptyString();
  testWithSingleComponent();
  testWithTwoComponents();
  testWithThreeComponents();
#if ADD_TEST
  testWithDiagram();
#endif
}
