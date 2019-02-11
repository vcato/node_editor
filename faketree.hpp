#ifndef FAKETREE_HPP_
#define FAKETREE_HPP_

#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "treewidget.hpp"


struct FakeSlider {
  int value = 0;
  int minimum = 0;
  int maximum = 100;
};


struct FakeTreeItem {
  bool is_expanded;
  std::string label;
  std::vector<FakeTreeItem> children;
  bool label_is_editable;
  std::unique_ptr<FakeSlider> value_widget_ptr;

  FakeTreeItem()
  : is_expanded(true),
    label_is_editable(false)
  {
  }

  int childCount() const { return children.size(); }
  FakeTreeItem *child(int index) { return &children[index]; }
  const FakeTreeItem *child(int index) const { return &children[index]; }

  friend FakeTreeItem& insertChildItem(FakeTreeItem &parent_item,int index)
  {
    parent_item.children.insert(
      parent_item.children.begin()+index,
      FakeTreeItem()
    );

    return parent_item.children[index];
  }

  friend void removeChildItem(FakeTreeItem &parent_item,int index)
  {
    parent_item.children.erase(parent_item.children.begin() + index);
  }

  friend void setText(FakeTreeItem &item,const std::string &text)
  {
    item.label = text;
  }

  FakeSlider *maybeSlider()
  {
    return value_widget_ptr.get();
  }
};


struct FakeTree : TreeWidget {
  using Item = FakeTreeItem;

  static FakeTree::Item &
    createItem(
      FakeTree &tree,
      const TreePath &new_item_path,
      const LabelProperties &label_properties
    );

  void
    createVoidItem(
      const TreePath &new_item_path,
      const TreeWidget::LabelProperties &label_properties
    ) override
  {
    createItem(
      *this,
      new_item_path,
      label_properties
    );
  }

  void
    createNumericItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties,
      NumericValue value,
      NumericValue minimum_value,
      NumericValue maximum_value
    )
  {
    FakeTreeItem &item =
      createItem(*this,new_item_path,label_properties);
    item.value_widget_ptr = std::make_unique<FakeSlider>();
    item.value_widget_ptr->value = value;
    item.value_widget_ptr->minimum = minimum_value;
    item.value_widget_ptr->maximum = maximum_value;
  }

  void
    createEnumerationItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties,
      const std::vector<std::string> &/*options*/,
      int /*value*/
    ) override
  {
    createItem(*this,new_item_path,label_properties);
  }

  void
    createStringItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties,
      const std::string &/*value*/
    ) override
  {
    createItem(*this,new_item_path,label_properties);
  }

  void setItemLabel(const TreePath &path,const std::string &new_label);

  void removeItem(const TreePath &path) override;

  void
    setItemNumericValue(
      const TreePath &,
      NumericValue value,
      NumericValue minimum_value,
      NumericValue maximum_value
    ) override;

  Item root;
};


#endif /* FAKETREE_HPP_ */
