#ifndef TREEPATH_HPP_
#define TREEPATH_HPP_

#include <vector>


using TreeItemIndex = int;
using TreePath = std::vector<TreeItemIndex>;


inline TreePath parentPath(const TreePath &path)
{
  TreePath result = path;
  result.pop_back();
  return result;
}


#endif /* TREEPATH_HPP_ */
