#include "wrapper.hpp"


// In this version, the wrapper is the wrapper for the given path.
extern Wrapper::PerformOperationFunction
  findAddBodyFunction(const Wrapper &wrapper,const TreePath &path);

// In this version, the wrapper is an ancestor and we want to find
// the function for the descendant given by the path.
extern Wrapper::PerformOperationFunction
  findAddBodyFunction2(const Wrapper &wrapper,const TreePath &path);
