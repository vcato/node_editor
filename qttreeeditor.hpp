#ifndef QTTREEEDITOR_HPP_
#define QTTREEEDITOR_HPP_

#include <QLabel>
#include <QString>
#include <QSpinBox>
#include <QTreeWidgetItem>
#include <QComboBox>
#include "qtwidget.hpp"
#include "qtdiagrameditor.hpp"
#include "qtcomboboxtreewidgetitemsignalmap.hpp"
#include "tree.hpp"


struct QtComboBoxTreeWidgetItem : QTreeWidgetItem {
  QtComboBoxTreeWidgetItem()
  : combo_box_ptr(0),
    signal_map(*this)
  {
  }

  QComboBox &comboBox()
  {
    assert(combo_box_ptr);
    return *combo_box_ptr;
  }

  QComboBox *combo_box_ptr;
  QtComboBoxTreeWidgetItemSignalMap signal_map;
};


class QtTreeEditor : public QTreeWidget {
  Q_OBJECT

public:
  using TreePath = Tree::Path;

  void setDiagramEditorPtr(QtDiagramEditor *arg) { diagram_editor_ptr = arg; }

  void setTreePtr(Tree *arg) { tree_ptr = arg; }

  template <typename T>
  T &setItemWidget(QTreeWidgetItem &item,const std::string &label)
  {
    QTreeWidget &tree_widget = *this;
    QWidget *wrapper_widget_ptr = new QWidget();
    QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);
    QLabel &label_widget = createWidget<QLabel>(layout);
    label_widget.setText(QString::fromStdString(label));
    T& widget = createWidget<T>(layout);
    tree_widget.setItemWidget(&item,/*column*/0,wrapper_widget_ptr);
    return widget;
  }

  QTreeWidgetItem& createItem(const std::string &label)
  {
    QTreeWidgetItem &item = createItem();
    setItemText(item,label);
    return item;
  }

  static QTreeWidgetItem&
    createItem(QTreeWidgetItem &parent_item,const std::string &label)
  {
    QTreeWidgetItem &pass_item = QtTreeEditor::createItem(parent_item);
    setItemText(pass_item,label);
    return pass_item;
  }

  static QTreeWidgetItem& createItem(QTreeWidgetItem &parent_item)
  {
    QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
    parent_item.addChild(item_ptr);
    QTreeWidgetItem &item = *item_ptr;
    item.setExpanded(true);
    return item;
  }

  QTreeWidgetItem& createItem()
  {
    QTreeWidget &tree_widget = *this;
    QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
    tree_widget.addTopLevelItem(item_ptr);
    item_ptr->setExpanded(true);
    return *item_ptr;
  }

  static void setItemText(QTreeWidgetItem &item,const std::string &label)
  {
    item.setText(/*column*/0,QString::fromStdString(label));
  }

  QtComboBoxTreeWidgetItem&
    createComboBoxItem(QTreeWidgetItem &parent_item,const std::string &label)
  {
    QtComboBoxTreeWidgetItem *item_ptr = new QtComboBoxTreeWidgetItem;
    parent_item.addChild(item_ptr);
    QtComboBoxTreeWidgetItem &item = *item_ptr;
    item.setExpanded(true);
    QComboBox &combo_box = setItemWidget<QComboBox>(item,label);
    item.combo_box_ptr = &combo_box;
    item.signal_map.connect(
      &combo_box,
      SIGNAL(currentIndexChanged(int)),
      SLOT(currentIndexChangedSlot(int))
    );
    connect(
      &item.signal_map,
      SIGNAL(currentIndexChanged(QtComboBoxTreeWidgetItem*,int)),
      SLOT(comboBoxItemCurrentIndexChangedSlot(QtComboBoxTreeWidgetItem*,int))
    );
    return item;
  }

  void
    createSpinBoxItem(
      QTreeWidgetItem &parent_item,
      const std::string &label
    )
  {
    QtTreeEditor &tree_widget = *this;
    QTreeWidgetItem &item = createItem(parent_item);
    tree_widget.setItemWidget<QSpinBox>(item,label);
  }

  QTreeWidgetItem* findSelectedItem();

  void
    addTreeItem(
      const TreePath &parent_path,
      const TreeItem &item
    );

  void
    addTreeItems(
      const TreePath &parent_path,
      const TreeItem &tree_items
    );

  void
    handleComboBoxItemIndexChanged(
      QtComboBoxTreeWidgetItem *item_ptr,
      int index
    );

  void handleAddPosExpr();

  void removeChildItems(const TreePath &path);

  void
    replaceTreeItems(
      const TreePath &parent_path,
      const TreeItem &tree_items
    );


  QtTreeEditor &treeEditor() { return *this; }

  std::vector<int> itemPath(QTreeWidgetItem &item);

  QTreeWidgetItem &itemFromPath(const std::vector<int> &path) const;

  private slots:
    void comboBoxItemCurrentIndexChangedSlot(QtComboBoxTreeWidgetItem *,int);

  signals:
    void comboBoxItemIndexChanged(QtComboBoxTreeWidgetItem*,int);

  private:
    bool ignore_combo_box_signals = false;
    Tree *tree_ptr = 0;
    Tree &tree();

    QtDiagramEditor &diagramEditor();
    QtDiagramEditor *diagram_editor_ptr = 0;
};

#endif /* QTTREEEDITOR_HPP_ */
