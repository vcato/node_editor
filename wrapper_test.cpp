#include "wrapper.hpp"

#include <cassert>

using std::string;


#if 0
namespace {
struct TestWrapper : VoidWrapper {
  virtual std::vector<OperationName> operationNames() const
  {
    return {};
  }

  virtual void
    executeOperation(
      int /*operation_index*/,
      const TreePath &/*path*/,
      OperationHandler &/*handler*/
    ) const
  {
    assert(false);
  }
};
}
#endif


#if 0
static TreePath makePath(const Wrapper &,const string &path_string)
{
  if (path_string=="") {
    assert(false);
  }
  else {
    assert(false);
  }
}
#endif


int main()
{
#if 0
  TestWrapper wrapper;
  makePath(wrapper,"");
#endif
}
