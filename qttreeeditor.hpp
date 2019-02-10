#ifndef QTTREEEDITOR_HPP_
#define QTTREEEDITOR_HPP_

#include <QTreeWidget>
#include "treeeditor.hpp"


class QtTreeEditor : public QTreeWidget, public TreeEditor {
  Q_OBJECT

  public:
    QtTreeEditor();

  private slots:
    void itemSelectionChangedSlot();
    void prepareMenuSlot(const QPoint &pos);

  private:
    QTreeWidgetItem &itemFromPath(const TreePath &path) const;
    TreePath itemPath(QTreeWidgetItem &item);
    void buildPath(TreePath &path,QTreeWidgetItem &item);
    void prepareMenu(const QPoint &pos);
    DiagramEditorWindow& createDiagramEditor() override;
    int itemChildCount(const TreePath &parent_item) const override;

    template <typename T>
    T &createItemWidget(
      QTreeWidgetItem &item,
      const LabelProperties &
    );

    static QTreeWidgetItem&
      createChildItem(QTreeWidgetItem &parent_item,const std::string &label);

    void
      createVoidItem(
        const TreePath &new_item_path,
        const LabelProperties &
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
      setItemNumericValue(
        const TreePath &,
        NumericValue value,
        NumericValue minimum_value,
        NumericValue maximum_value
      ) override;

    void
      createEnumerationItem(
        const TreePath &new_item_path,
        const LabelProperties &,
        const std::vector<std::string> &options,
        int value
      ) override;

    void
      createStringItem(
        const TreePath &new_item_path,
        const LabelProperties &,
        const std::string &value
      ) override;

    void
      setItemLabel(const TreePath &path,const std::string &new_label) override;

    static void setItemText(QTreeWidgetItem &item,const std::string &label);

    QTreeWidgetItem&
      createComboBoxItem(
        QTreeWidgetItem &parent_item,
        int index,
        const LabelProperties &,
        const std::vector<std::string> &enumeration_names,
        int value
      );

    void
      createLineEditItem(
        QTreeWidgetItem &,
        const LabelProperties &,
        const std::string &value
      );

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

    QTreeWidgetItem* findSelectedItem();

    void removeTreeItem(const TreePath &path) override;
    void removeChildItems(const TreePath &path) override;
    void setItemExpanded(const TreePath &path,bool new_expanded_state) override;

    void
      handleComboBoxItemIndexChanged(
        QTreeWidgetItem *item_ptr,
        int index
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

    void selectItem(const TreePath &path);

    void itemLabelChanged(QTreeWidgetItem &,const std::string &new_text);
};

#endif /* QTTREEEDITOR_HPP_ */
