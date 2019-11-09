#ifndef QTTREEEDITOR_HPP_
#define QTTREEEDITOR_HPP_

#include <QTreeWidget>
#include "treeeditor.hpp"
#include "qttreewidget.hpp"
#include "qtslider.hpp"
#include "qtspinbox.hpp"


class QtTreeEditor : public QtTreeWidget, public TreeEditor {
  Q_OBJECT

  public:
    QtTreeEditor();

  private slots:
    void itemSelectionChangedSlot();

  private:
    using LabelProperties = TreeWidget::LabelProperties;

    DiagramEditorWindow& createDiagramEditor() override;
    int itemChildCount(const TreePath &parent_item) const override;
    QTreeWidgetItem* findSelectedItem();
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

    QtTreeWidget &tree() { return *this; }
    const QtTreeWidget &tree() const { return *this; }
};

#endif /* QTTREEEDITOR_HPP_ */
