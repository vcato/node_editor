#include <QTreeWidget>

#include "qtslider.hpp"
#include "qtspinbox.hpp"
#include "treewidget.hpp"

struct QHBoxLayout;
struct QLabel;


struct QtTreeWidget : QTreeWidget, TreeWidget {
  void
    createVoidItem(
      const TreePath &new_item_path,
      const TreeWidget::LabelProperties &label_properties
    ) override;

  void
    createNumericItem(
      const TreePath &new_item_path,
      const LabelProperties &,
      const NumericValue value,
      const NumericValue minimum_value,
      const NumericValue maximum_value
    ) override;

  void
    createSpinBoxItem(
      QTreeWidgetItem &parent_item,
      int child_index,
      const LabelProperties &,
      int value,
      int minimum_value,
      int maximum_value
    );

  void
    createSliderItem(
      QTreeWidgetItem &parent_item,
      int child_index,
      const LabelProperties &,
      int value,
      int minimum_value,
      int maximum_value
    );

  void
    createEnumerationItem(
      const TreePath &new_item_path,
      const LabelProperties &label_properties,
      const std::vector<std::string> &options,
      int value
    ) override;

  void
    setItemNumericValue(
      const TreePath &,
      NumericValue value,
      NumericValue minimum_value,
      NumericValue maximum_value
    ) override;

  void
    setItemLabel(const TreePath &path,const std::string &new_label) override;

  void changeItemToSlider(const TreePath &path);
  void changeItemToSpinBox(const TreePath &path);
  QtSlider* itemSliderPtr(const TreePath &path);
  QtSpinBox* itemSpinBoxPtr(const TreePath &path);
  QLabel *itemLabelPtr(const TreePath &path);

  QTreeWidgetItem&
    createComboBoxItem(
      QTreeWidgetItem &parent_item,
      int index,
      const LabelProperties &,
      const std::vector<std::string> &enumeration_names,
      int value
    );

  void
    createStringItem(
      const TreePath &new_item_path,
      const LabelProperties &,
      const std::string &value
    ) override;

  void
    createLineEditItem(
      QTreeWidgetItem &,
      const LabelProperties &,
      const std::string &value
    );

  void
    handleComboBoxItemIndexChanged(
      QTreeWidgetItem *item_ptr,
      int index
    );

  void
    handleSliderItemValueChanged(
      QTreeWidgetItem *item_ptr,
      int value
    );

  void
    handleSpinBoxItemValueChanged(
      QTreeWidgetItem *item_ptr,
      int value
    );

  void
    handleLineEditItemValueChanged(
      QTreeWidgetItem *item_ptr,
      const std::string &value
    );

  TreePath itemPath(QTreeWidgetItem &item);

  void buildPath(TreePath &path,QTreeWidgetItem &item);

  template <typename T>
  T &createItemWidget(
    QTreeWidgetItem &item,
    const LabelProperties &
  );

  QLabel&
    createItemLabelWidget(
      QTreeWidgetItem &item,
      QHBoxLayout &layout,
      const LabelProperties &label_properties
    );

  QTreeWidgetItem &itemFromPath(const std::vector<int> &path) const;
  static void setItemText(QTreeWidgetItem &item,const std::string &label);

  std::function<void(const TreePath &,int index)>
    combobox_item_index_changed_function;

  std::function<void(const TreePath &,int index)>
    spin_box_item_value_changed_function;

  std::function<void(const TreePath &,int index)>
    slider_item_value_changed_function;

  std::function<void(const TreePath &,const std::string &value)>
    line_edit_item_value_changed_function;

  static QTreeWidgetItem&
    createChildItem(QTreeWidgetItem &parent_item,const std::string &label);

  void removeItem(const TreePath &path) override;

  struct Impl;
};
