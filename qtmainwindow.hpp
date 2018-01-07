#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QTreeWidget>
#include <QBoxLayout>
#include <QSplitter>
#include "qtdiagrameditor.hpp"
#include "tree.hpp"
#include "qttreeeditor.hpp"


class QtMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    QtMainWindow();

  private:
    using TreePath = Tree::Path;
    QMenu menu{"Tools"};
    Tree tree_member;
    Tree &tree() { return tree_member; }
    QtTreeEditor *tree_editor_ptr;
    QtDiagramEditor *diagram_editor_ptr;

    QtTreeEditor &treeEditor();
    void createTree(QSplitter &parent_splitter);
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

    std::vector<int> itemPath(QTreeWidgetItem &item);
};
