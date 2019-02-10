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


void
  FakeTreeEditor::createNumericItem(
    const TreePath &new_item_path,
    const LabelProperties &label_properties,
    const NumericValue value,
    const NumericValue minimum_value,
    const NumericValue maximum_value
  )
{
  FakeTreeItem &item =
    createItem(tree,new_item_path,label_properties);
  item.value_widget_ptr = std::make_unique<FakeSlider>();
  item.value_widget_ptr->value = value;
  item.value_widget_ptr->minimum = minimum_value;
  item.value_widget_ptr->maximum = maximum_value;
}


void
  FakeTreeEditor::setItemNumericValue(
    const TreePath &path,
    NumericValue value,
    NumericValue minimum_value,
    NumericValue maximum_value
  )
{
  FakeTreeItem &item = itemFromPath(tree.root,path);
  assert(item.value_widget_ptr);
  item.value_widget_ptr->value = value;
  item.value_widget_ptr->minimum = minimum_value;
  item.value_widget_ptr->maximum = maximum_value;
}
