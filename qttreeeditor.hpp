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
#include "qtslot.hpp"
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

    QtTreeEditor();

    void setTreePtr(Tree *arg) { tree_ptr = arg; }
    void setDiagramEditorPtr(QtDiagramEditor *arg) { diagram_editor_ptr = arg; }
    void selectItem(const TreePath &path);

  private slots:
    void comboBoxItemCurrentIndexChangedSlot(QtComboBoxTreeWidgetItem *,int);
    void itemSelectionChangedSlot();
    void prepareMenuSlot(const QPoint &pos);

  private:
    struct OperationHandler : TreeOperationHandler {
      QtTreeEditor &tree_editor;

      OperationHandler(QtTreeEditor &tree_editor_arg)
      : tree_editor(tree_editor_arg)
      {
      }

      virtual void addItem(const TreePath &path,const TreeItem &item)
      {
        tree_editor.addTreeItem(path,item);
      }
    };

    bool ignore_combo_box_signals = false;
    Tree *tree_ptr = 0;
    QtDiagramEditor *diagram_editor_ptr = 0;
    OperationHandler operation_handler;

    Tree &tree();
    QtDiagramEditor &diagramEditor();
    QtTreeEditor &treeEditor() { return *this; }
    Diagram *maybeSelectedDiagram();
    QTreeWidgetItem &itemFromPath(const TreePath &path) const;
    TreePath itemPath(QTreeWidgetItem &item);
    void buildPath(TreePath &path,QTreeWidgetItem &item);
    void prepareMenu(const QPoint &pos);

    Tree::OperationVisitor
      addMenuItemForOperationFunction(
        QMenu &menu,
        std::list<QtSlot> &item_slots
      );

    template <typename T>
    T &createItemWidget(QTreeWidgetItem &item,const std::string &label)
    {
      QWidget *wrapper_widget_ptr = new QWidget();
      QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);
      QLabel &label_widget = createWidget<QLabel>(layout);
      label_widget.setText(QString::fromStdString(label));
      T& widget = createWidget<T>(layout);
      setItemWidget(&item,/*column*/0,wrapper_widget_ptr);
      return widget;
    }

    static QTreeWidgetItem&
      createItem(QTreeWidgetItem &parent_item,const std::string &label);

    static QTreeWidgetItem& createItem(QTreeWidgetItem &parent_item);

    static void setItemText(QTreeWidgetItem &item,const std::string &label);

    QtComboBoxTreeWidgetItem&
      createComboBoxItem(QTreeWidgetItem &parent_item,const std::string &label);

    void
      createSpinBoxItem(QTreeWidgetItem &parent_item,const std::string &label);

    QTreeWidgetItem* findSelectedItem();

    void addTreeItem(const TreePath &parent_path, const TreeItem &item);
    void addTreeItems(const TreePath &parent_path,const TreeItem &tree_items);

    void
      handleComboBoxItemIndexChanged(
        QtComboBoxTreeWidgetItem *item_ptr,
        int index
      );

    void removeChildItems(const TreePath &path);

    void
      replaceTreeItems(
        const TreePath &parent_path,
        const TreeItem &tree_items
      );
};

#endif /* QTTREEEDITOR_HPP_ */
