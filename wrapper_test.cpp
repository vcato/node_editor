#include "wrapper.hpp"

#include <cassert>

using std::string;


namespace {
struct TestWrapper : NoOperationWrapper<LeafWrapper<VoidWrapper>> {
};
}


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
