#include "wrapperstate.hpp"

#include <sstream>
#include "wrapper.hpp"


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

  void
    setState(
      const WrapperState &,
      const TreePath &,
      TreeObserver &
    ) const override
  {
    assert(false);
  }
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


static void testScanStateFromWithEnumerationValue()
{
  istringstream stream("test: b\n");
  ScanStateResult result = scanStateFrom(stream);

  assert(result.isState());
  const WrapperState &state = result.state();

  assert(state.tag=="test");
  assert(state.value==WrapperValue(WrapperValue::Enumeration{"b"}));
  assert(state.children.empty());
}


static void testScanStateFromWithStringValue()
{
  istringstream stream("test: \"quoted\"\n");
  ScanStateResult result = scanStateFrom(stream);

  assert(result.isState());
  const WrapperState &state = result.state();

  assert(state.tag=="test");
  assert(state.value==WrapperValue("quoted"));
  assert(state.children.empty());
}


static void testScanStateFromWithChildValues()
{
  const char *text =
    "test {\n"
    "  a: 5\n"
    "}\n";
  istringstream stream(text);
  ScanStateResult result = scanStateFrom(stream);

  assert(result.isState());
  const WrapperState &state = result.state();

  assert(state.tag=="test");
  assert(state.value==WrapperValue(WrapperValue::Void{}));
  assert(state.children.size()==1);
  assert(state.children[0].tag=="a");
  assert(state.children[0].value.isNumeric());
  assert(state.children[0].value==WrapperValue(5));
}


static void testPrintAndScanStateWithNoChildren()
{
  WrapperState state("top");
  ostringstream stream;
  printStateOn(stream,state);
  string text = stream.str();
  istringstream input_stream(text);
  ScanStateResult scan_result = scanStateFrom(input_stream);
  assert(!scan_result.isError());
}


int main()
{
  testPrintStateOn();
  testScanStateFromWithEnumerationValue();
  testScanStateFromWithStringValue();
  testScanStateFromWithChildValues();
  testPrintAndScanStateWithNoChildren();
}
