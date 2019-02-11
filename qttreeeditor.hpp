#ifndef QTTREEEDITOR_HPP_
#define QTTREEEDITOR_HPP_

#include <QTreeWidget>
#include "treeeditor.hpp"

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
};


class QtTreeEditor : public QtTreeWidget, public TreeEditor {
  Q_OBJECT

  public:
    QtTreeEditor();

  private slots:
    void itemSelectionChangedSlot();
    void prepareMenuSlot(const QPoint &pos);

  private:
    using LabelProperties = TreeWidget::LabelProperties;
    QTreeWidgetItem &itemFromPath(const TreePath &path) const;

    void prepareMenu(const QPoint &pos);
    DiagramEditorWindow& createDiagramEditor() override;
    int itemChildCount(const TreePath &parent_item) const override;

    static QTreeWidgetItem&
      createChildItem(QTreeWidgetItem &parent_item,const std::string &label);

    void
      setItemNumericValue(
        const TreePath &,
        NumericValue value,
        NumericValue minimum_value,
        NumericValue maximum_value
      ) override;

    void
      setItemLabel(const TreePath &path,const std::string &new_label) override;

    static void setItemText(QTreeWidgetItem &item,const std::string &label);

    QTreeWidgetItem* findSelectedItem();

    void removeItem(const TreePath &path) override;
    void removeChildItems(const TreePath &path) override;
    void setItemExpanded(const TreePath &path,bool new_expanded_state) override;

    void treeComboBoxItemIndexChanged(const TreePath &path,int index);
    void treeSpinBoxItemValueChanged(const TreePath &path,int value);

    void
      treeSliderItemValueChanged(
        const TreePath &path,
        int value
      );

    void
      treeLineEditItemValueChanged(
        const TreePath &path,
        const std::string &value
      );

    void selectItem(const TreePath &path);

    QtTreeWidget &tree() { return *this; }
    const QtTreeWidget &tree() const { return *this; }
};

#endif /* QTTREEEDITOR_HPP_ */
