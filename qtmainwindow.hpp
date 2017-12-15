#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QTreeWidget>
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
    QTreeWidgetItem *charmapper_item_ptr;
    QTreeWidgetItem *motion_pass_item_ptr;
    QTreeWidgetItem *add_pos_expr_item_ptr;

    QTreeWidget &treeWidget();
    QTreeWidgetItem* findSelectedItem();
};
