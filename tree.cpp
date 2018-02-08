#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include "defaultdiagrams.hpp"
#include "streamvector.hpp"


using std::cerr;

Diagram *Wrapper::diagramPtr(const TreePath &path)
{
  Diagram *result_ptr = 0;

  visitWrapper(
    path,
    [&result_ptr](const Wrapper &wrapper){ result_ptr = wrapper.diagramPtr(); }
  );

  if (!result_ptr) {
    cerr << "No diagram found for " << path << "\n";
    return nullptr;
  }

  return result_ptr;
}
