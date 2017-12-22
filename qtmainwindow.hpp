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

  private slots:
    void prepareMenu(const QPoint &pos);
    void treeItemSelectionChanged();
    void addPassTriggered();
    void addPosExprTriggered();
    void treeComboBoxItemIndexChanged(QtComboBoxTreeWidgetItem *,int);

  private:
    QMenu menu{"Tools"};
    Tree tree;
    Diagram diagram;
    QWidget widget;
    QtTreeWidget *tree_widget_ptr;
    QtDiagramEditor *diagram_editor_ptr;

    QtTreeWidget &treeWidget();
    void createTree(QBoxLayout &layout);
    QTreeWidgetItem* findSelectedItem();
};
