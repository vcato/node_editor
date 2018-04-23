#include "wrapper.hpp"

#include <cassert>
#include "wrapperutil.hpp"

using std::string;
using std::vector;
using std::cerr;
using std::function;


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


int main()
{
  testWithEmptyString();
  testWithSingleComponent();
  testWithTwoComponents();
  testWithThreeComponents();
}
