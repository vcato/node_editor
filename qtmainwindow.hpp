#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QTreeWidget>
#include <QBoxLayout>
#include "qtdiagrameditor.hpp"
#include "tree.hpp"
#include "qttreewidget.hpp"


class QtMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    QtMainWindow();

  private:
    using TreePath = Tree::Path;
    QMenu menu{"Tools"};
    Tree tree;
    QWidget widget;
    QtTreeWidget *tree_widget_ptr;
    QtDiagramEditor *diagram_editor_ptr;
    bool ignore_combo_box_signals = false;

    QtTreeWidget &treeWidget();
    void createTree(QBoxLayout &layout);
    QTreeWidgetItem* findSelectedItem();
    Diagram *selectedDiagramPtr();
    QtDiagramEditor &diagramEditor();

  private slots:
    void prepareMenu(const QPoint &pos);
    void treeItemSelectionChanged();
    void addPassTriggered();
    void addPosExprTriggered();
    void treeComboBoxItemIndexChanged(QtComboBoxTreeWidgetItem *,int);
    void addTreeItem(const TreePath &parent_path,const TreeItem &item);
    void addTreeItems(const TreePath &,const TreeItem &);
    void removeChildItems(const TreePath &parent_path);
    void
      replaceTreeItems(
        const TreePath &parent_path,
        const TreeItem &tree_items
      );
};
