#include "faketreeeditor.hpp"

#include <algorithm>
#include "streamvector.hpp"


static int
  findIndex(
    const std::vector<std::string> &container,
    const std::string &element
  )
{
  auto b = container.begin();
  auto e = container.end();
  auto iter = std::find(b,e,element);

  if (iter==e) {
    std::cerr << "Could not find " << element << " in " << container << "\n";
  }

  assert(iter!=e);
  return iter-b;
}


void
  FakeTreeEditor::userSelectsContextMenuItem(
    const std::string &operation_name
  )
{
  TreePath path = {};
  std::vector<std::string> operation_names = operationNames(path);
  int index = findIndex(operation_names,operation_name);
  executeOperation(path,index);
}
