#include "faketree.hpp"

#include <iostream>
#include "itemfrompath.hpp"
#include "insertitemin.hpp"
#include "printindent.hpp"
#include "ostreamvector.hpp"
#include "wrappervaluetypes.hpp"


using std::string;
using std::ostream;
using std::make_unique;
using std::cerr;


void FakeSlider::printOn(std::ostream &stream)
{
  stream << "Slider(value=" << value;

  if (minimum != noNumericMinimum()) {
    stream << ", minimum=" << minimum;
  }

  if (maximum != noNumericMaximum()) {
    stream << ", maximum=" << maximum;
  }

  stream << ")";
}


void FakeComboBox::printOn(std::ostream &stream)
{
  stream << "ComboBox(options=" << options << ",value=" << value << ")";
}


FakeSlider *FakeTreeItem::maybeSlider()
{
  return dynamic_cast<FakeSlider*>(value_widget_ptr.get());
}


FakeTree::Item &
  FakeTree::createItem(
    FakeTree &tree,
    const TreePath &new_item_path,
    const LabelProperties &label_properties
  )
{
  TreePath parent_path = parentPath(new_item_path);
  FakeTree::Item &parent_item = itemFromPath(tree.root, parent_path);
  int new_item_index = new_item_path.back();
  FakeTree::Item &new_item =
    insertItemIn(parent_item.children,new_item_index);
  new_item.label = label_properties.text;
  return new_item;
}


void
  FakeTree::createNumericItem(
    const TreePath &new_item_path,
    const LabelProperties &label_properties,
    NumericValue value,
    NumericValue minimum_value,
    NumericValue maximum_value
  )
{
  FakeTreeItem &item =
    createItem(*this,new_item_path,label_properties);

  auto fake_slider_ptr = make_unique<FakeSlider>();
  fake_slider_ptr->value = value;
  fake_slider_ptr->minimum = minimum_value;
  fake_slider_ptr->maximum = maximum_value;

  item.value_widget_ptr = std::move(fake_slider_ptr);
}


void
  FakeTree::createEnumerationItem(
    const TreePath &new_item_path,
    const LabelProperties &label_properties,
    const std::vector<std::string> &options,
    int value
  )
{
  FakeTree::Item &item = createItem(*this,new_item_path,label_properties);
  auto combobox_ptr = make_unique<FakeComboBox>();
  combobox_ptr->options = options;
  combobox_ptr->value = options[value];
  item.value_widget_ptr = std::move(combobox_ptr);
}


void FakeTree::removeItem(const TreePath &path)
{
  Item &parent_item = itemFromPath(root, parentPath(path));
  removeChildItem(parent_item,path.back());
}


void FakeTree::setItemLabel(const TreePath &path,const std::string &new_label)
{
  itemFromPath(root, path).label = new_label;
}


void FakeTree::setItemNumericValue( const TreePath &path, NumericValue value )
{
  FakeTreeItem &item = itemFromPath(root,path);
  assert(item.maybeSlider());
  item.maybeSlider()->value = value;
}


void
  FakeTree::setItemNumericValue(
    const TreePath &path,
    NumericValue value,
    NumericValue minimum_value,
    NumericValue maximum_value
  )
{
  FakeTreeItem &item = itemFromPath(root,path);
  assert(item.maybeSlider());
  item.maybeSlider()->value = value;
  item.maybeSlider()->minimum = minimum_value;
  item.maybeSlider()->maximum = maximum_value;
}


static void
  printChildrenOn(ostream &stream,const FakeTreeItem &item,int indent_level)
{
  for (auto &child : item.children) {
    const string &label = child.label;
    printIndent(stream,indent_level);
    stream << label;

    if (child.value_widget_ptr) {
      stream << ": ";
      child.value_widget_ptr->printOn(stream);
    }

    stream << "\n";

    if (!child.children.empty()) {
      printChildrenOn(stream,child,indent_level + 1);
    }
  }
}


void FakeTree::printOn(ostream &stream) const
{
  printChildrenOn(stream,root,/*indent_level*/0);
}
