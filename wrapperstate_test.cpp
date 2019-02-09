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
      Index,
      const TreePath &,
      TreeObserver &
    ) const
  {
    assert(false);
  }

  virtual Index value() const { return 1; }

  void setState(const WrapperState &) const override
  {
    assert(false);
  }
};
}


static WrapperState stateFromText(const char *text)
{
  istringstream stream(text);
  ScanStateResult scan_result = scanStateFrom(stream);
  assert(scan_result.isState());
  return scan_result.state();
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
  WrapperState state = stateFromText("test: b\n");

  assert(state.tag=="test");
  assert(state.value==WrapperValue(WrapperValue::Enumeration{"b"}));
  assert(state.children.empty());
}


static void testScanStateFromWithEnumerationValueAndChildren()
{
  const char *text =
    "test: b {\n"
    "  x: 5\n"
    "}\n";
  WrapperState state = stateFromText(text);

  assert(state.tag=="test");
  assert(state.value==WrapperValue(WrapperValue::Enumeration{"b"}));
  assert(state.children.size()==1);
  assert(state.children[0].tag=="x");
  assert(state.children[0].value==5);
}


static void testScanStateFromWithStringValue()
{
  WrapperState state = stateFromText("test: \"quoted\"\n");

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
  WrapperState state = stateFromText(text);

  assert(state.tag=="test");
  assert(state.value==WrapperValue(WrapperValue::Void{}));
  assert(state.children.size()==1);
  assert(state.children[0].tag=="a");
  assert(state.children[0].value.isNumeric());
  assert(state.children[0].value==WrapperValue(5));
}


static void testScanStateFromWithError()
{
  const char *text =
    "test {\n"
    "#}\n";
  istringstream stream(text);
  ScanStateResult result = scanStateFrom(stream);
  assert(result.isError());
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


static void testScanCase1()
{
  const char *text =
    "pos_expr {\n"
    "  global_position: from_body {\n"
    "  }\n"
    "  diagram {\n"
    "  }\n"
    "}\n";
  WrapperState state = stateFromText(text);
  assert(state.children.size()==2);
}


int main()
{
  testPrintStateOn();
  testScanStateFromWithEnumerationValue();
  testScanStateFromWithEnumerationValueAndChildren();
  testScanStateFromWithStringValue();
  testScanStateFromWithChildValues();
  testScanStateFromWithError();
  testScanCase1();
  testPrintAndScanStateWithNoChildren();
}
