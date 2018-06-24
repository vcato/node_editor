#include "wrapperstate.hpp"

#include <sstream>


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


int main()
{
  TestWrapper wrapper;
  WrapperState state = stateOf(wrapper);
  ostringstream stream;
  printStateOn(stream,state);
  string text = stream.str();
  string expected_text = "test: b\n";
  assert(text==expected_text);
}
