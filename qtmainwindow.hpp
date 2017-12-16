#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QTreeWidget>
#include <QBoxLayout>
#include "qtdiagrameditor.hpp"
#include "tree.hpp"


class QtMainWindow : public QMainWindow {
  Q_OBJECT

  public:
    QtMainWindow();

  private slots:
    void prepareMenu(const QPoint &pos);
    void addPassTriggered();
    void addPosExprTriggered();

  private:
    QMenu menu{"Tools"};
    Tree tree;
    Diagram diagram;
    QWidget widget;
    QTreeWidget *tree_widget_ptr;

    QTreeWidget &treeWidget();
    void createTree(QBoxLayout &layout);
    QTreeWidgetItem* findSelectedItem();
};
