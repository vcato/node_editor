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
    QtTreeEditor &treeEditor() { return *this; }
    QTreeWidgetItem &itemFromPath(const TreePath &path) const;
    TreePath itemPath(QTreeWidgetItem &item);
    void buildPath(TreePath &path,QTreeWidgetItem &item);
    void prepareMenu(const QPoint &pos);
    DiagramEditorWindow& createDiagramEditor() override;
    int itemChildCount(const TreePath &parent_item) const override;

    template <typename T>
    T &createItemWidget(QTreeWidgetItem &item,const std::string &label);

    static QTreeWidgetItem&
      createChildItem(QTreeWidgetItem &parent_item,const std::string &label);

    void
      createVoidItem(
        const TreePath &parent_path,
        const TreePath &new_item_path,
        const std::string &label
      ) override;

    void
      createNumericItem(
        const TreePath &parent_path,
        const std::string &label,
        const NumericValue value
      ) override;

    void
      createEnumerationItem(
        const TreePath &parent_path,
        const TreePath &new_item_path,
        const std::string &label,
        const std::vector<std::string> &options,
        int value
      ) override;

    void
      createStringItem(
        const TreePath &parent_path,
        const std::string &label,
        const std::string &value
      ) override;

    static void setItemText(QTreeWidgetItem &item,const std::string &label);

    QTreeWidgetItem&
      createComboBoxItem(
        QTreeWidgetItem &parent_item,
        int index,
        const std::string &label,
        const std::vector<std::string> &enumeration_names,
        int value
      );

    void
      createLineEditItem(
        QTreeWidgetItem &,
        const std::string &label,
        const std::string &value
      );

    void
      createSpinBoxItem(
        QTreeWidgetItem &parent_item,
        const std::string &label,
        int value
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

    void
      setEnumerationValues(
        const TreePath &path,
        const std::vector<std::string> &items
      ) override;

    void selectItem(const TreePath &path);
};

#endif /* QTTREEEDITOR_HPP_ */
