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
    struct ItemSpec {
      virtual void
        createChildItem(
          QtTreeEditor &,
          const TreePath &parent_path
	) const = 0;
    };

    struct VoidItemSpec;

#if 1
    struct CreateChildItemVisitor;
#else
    struct CreateItemVisitor;
#endif
    struct OperationHandler;

    bool ignore_combo_box_signals = false;
    Tree *tree_ptr = 0;
    QtDiagramEditor *diagram_editor_ptr = 0;

    Tree &tree();
    QtDiagramEditor &diagramEditor();
    QtTreeEditor &treeEditor() { return *this; }
    Diagram *maybeSelectedDiagram();
    QTreeWidgetItem &itemFromPath(const TreePath &path) const;
    TreePath itemPath(QTreeWidgetItem &item);
    void buildPath(TreePath &path,QTreeWidgetItem &item);
    void prepareMenu(const QPoint &pos);

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
      createChildItem(QTreeWidgetItem &parent_item,const std::string &label);

    static QTreeWidgetItem& createChildItem(QTreeWidgetItem &parent_item);

    static void setItemText(QTreeWidgetItem &item,const std::string &label);

    QtComboBoxTreeWidgetItem&
      createComboBoxItem(QTreeWidgetItem &parent_item,const std::string &label);

    void
      createSpinBoxItem(QTreeWidgetItem &parent_item,const std::string &label);

    QTreeWidgetItem* findSelectedItem();

    void addTreeChildItem(const TreePath &parent_path);
    void addTreeItems(const TreePath &parent_path);

    void
      handleComboBoxItemIndexChanged(
        QtComboBoxTreeWidgetItem *item_ptr,
        int index
      );

    void removeChildItems(const TreePath &path);

    void replaceTreeItems(const TreePath &parent_path);

    void
      createVoidChildItem(
        const TreePath &parent_path,
        const std::string &label
      );

    void createItem(const TreePath &path,const ItemSpec &spec);

    void
      createVoidItem(
	const TreePath &path,
	const std::string &label
      );

    void
      createNumericChildItem(
        const TreePath &parent_path,
        const std::string &label
      );

    void
      createEnumeratedChildItem(
        const TreePath &parent_path,
        const std::string &label,
        const std::vector<std::string> &enumeration_names
      );
};

#endif /* QTTREEEDITOR_HPP_ */
