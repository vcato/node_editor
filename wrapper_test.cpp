#include "wrapper.hpp"

#include <cassert>
#include <iostream>
#include "wrapperutil.hpp"
#include "diagram.hpp"

using std::string;
using std::vector;
using std::function;
using std::cerr;


namespace {
struct ComponentWrapper : NoOperationWrapper<LeafWrapper<VoidWrapper>> {
};
}


namespace {
struct ZWrapper : ComponentWrapper {
  virtual Label label() const { return "Z"; }

  using ComponentWrapper::ComponentWrapper;
};
}


namespace {
struct YWrapper : ComponentWrapper {
  using ComponentWrapper::ComponentWrapper;

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
struct XWrapper : ComponentWrapper {
  using ComponentWrapper::ComponentWrapper;

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
};
}


namespace {
struct TestWrapper1 : TestWrapper {
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
struct TestWrapper2 : TestWrapper {
  vector<TestWrapper2> children;
  string label_member;
  Diagram *diagram_ptr = nullptr;
  Diagram *default_diagram_ptr = nullptr;

  void setDiagramPtr(Diagram *arg)
  {
    diagram_ptr = arg;
  }

  void setDefaultDiagramPtr(Diagram *arg)
  {
    default_diagram_ptr = arg;
  }

  Diagram *diagramPtr() const override
  {
    return diagram_ptr;
  }

  const Diagram &defaultDiagram() const override
  {
    assert(default_diagram_ptr);
    return *default_diagram_ptr;
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
  TestWrapper1 wrapper;
  TreePath result = makePath(wrapper,"");
  assert(result.empty());
}


static void testWithSingleComponent()
{
  TreePath result = makePath(TestWrapper1(),"X");
  TreePath expected_result = {0};
  assert(result==expected_result);
}


static void testWithTwoComponents()
{
  TreePath result = makePath(TestWrapper1(),"X|Y");
  TreePath expected_result = {0,0};
  assert(result==expected_result);
}


static void testWithThreeComponents()
{
  TreePath result = makePath(TestWrapper1(),"X|Y|Z");
  TreePath expected_result = {0,0,0};
  assert(result==expected_result);
}


static void testWithDiagram()
{
  // If we have a diagram that is different than the default diagram, then
  // we should get the diagram as part of the wrapper state.

  Diagram diagram;
  Diagram default_diagram;
  default_diagram.createNodeWithText("5");
  TestWrapper2 wrapper;
  wrapper.setDiagramPtr(&diagram);
  wrapper.setDefaultDiagramPtr(&default_diagram);
  WrapperState state = stateOf(wrapper);
  assert(state.children.size()==1);
  assert(state.children[0].tag=="diagram");
}


int main()
{
  testWithEmptyString();
  testWithSingleComponent();
  testWithTwoComponents();
  testWithThreeComponents();
  testWithDiagram();
}
