#include "wrapperstate.hpp"

#include <sstream>


using std::istringstream;
using std::ostringstream;
using std::string;
using std::cerr;


namespace {
struct TestWrapper : LeafWrapper<NoOperationWrapper<EnumerationWrapper> > {
  virtual Label label() const { return "test"; }

  virtual std::vector<std::string> enumerationNames() const
  {
    return {"a","b","c"};
  }

  virtual void
    setValue(
      const TreePath &,
      Index,
      TreeObserver &
    ) const
  {
    assert(false);
  }

  virtual Index value() const { return 1; }
};
}


static void testPrintStateOn()
{
  TestWrapper wrapper;
  WrapperState state = stateOf(wrapper);
  ostringstream stream;
  printStateOn(stream,state);
  string text = stream.str();
  string expected_text = "test: b\n";
  assert(text==expected_text);
}


#if 0
static void testScanStateFrom()
{
  istringstream stream("test: b\n");
  ScanStateResult result = scanStateFrom(stream);

  assert(result.isState());
  const WrapperState &state = result.state();

  assert(state.tag=="test");
  assert(state.value==WrapperValue("b"));
  assert(state.children.empty());
}
#endif


int main()
{
  testPrintStateOn();
  // testScanStateFrom();
}
