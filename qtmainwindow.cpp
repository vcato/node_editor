#include "qtmainwindow.hpp"

#include <cassert>
#include <iostream>
#include <QMenuBar>
#include <QBoxLayout>
#include <QHeaderView>
#include <QPushButton>


using std::cerr;
using std::string;


template <typename Layout>
static Layout& setLayout(QWidget &widget)
{
  Layout *layout_ptr = new Layout;
  widget.setLayout(layout_ptr);
  return *layout_ptr;
}


template <typename Layout>
static Layout& addLayoutTo(QBoxLayout &parent_layout)
{
  Layout *layout_ptr = new Layout;
  parent_layout.addLayout(layout_ptr);
  return *layout_ptr;
}


template <typename Widget>
static Widget& addTo(QLayout &layout,Widget *widget_ptr)
{
  layout.addWidget(widget_ptr);
  return *widget_ptr;
}


static QTreeWidget& addTreeWidgetTo(QLayout &layout)
{
  return addTo(layout,new QTreeWidget);
}


static QPushButton& addPushButtonTo(QLayout &layout)
{
  return addTo(layout,new QPushButton);
}


static QAction& addActionTo(QMenu &menu,const string &label)
{
  QAction *add_pass_action_ptr = new QAction(QString::fromStdString(label),0);
  menu.addAction(add_pass_action_ptr);
  return *add_pass_action_ptr;
}


static void addDiagramEditorTo(QLayout &layout,Diagram &diagram)
{
  QtDiagramEditor *diagram_editor_ptr = new QtDiagramEditor(diagram);
  layout.addWidget(diagram_editor_ptr);
}


static void setItemText(QTreeWidgetItem &item,const string &label)
{
  item.setText(/*column*/0,QString::fromStdString(label));
}


static QTreeWidgetItem& addItemTo(QTreeWidget &tree_widget,const string &label)
{
  QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
  setItemText(*item_ptr,label);
  tree_widget.addTopLevelItem(item_ptr);
  item_ptr->setExpanded(true);
  return *item_ptr;
}


static QTreeWidgetItem&
  addChildItemTo(QTreeWidgetItem &parent_item,const string &label)
{
  QTreeWidgetItem *pass_item_ptr = new QTreeWidgetItem;
  setItemText(*pass_item_ptr,label);
  parent_item.addChild(pass_item_ptr);
  QTreeWidgetItem &pass_item = *pass_item_ptr;
  pass_item.setExpanded(true);
  return pass_item;
}


QtMainWindow::QtMainWindow()
: tree_widget_ptr(0),
  charmapper_item_ptr(0),
  motion_pass_item_ptr(0),
  add_pos_expr_item_ptr(0)
{
  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  menu_bar_ptr->addMenu(&menu);

  QHBoxLayout &layout = ::setLayout<QHBoxLayout>(widget);
  {
    QVBoxLayout &layout2 = addLayoutTo<QVBoxLayout>(layout);

    {
      QTreeWidget &tree_widget = addTreeWidgetTo(layout2);
      tree_widget_ptr = &tree_widget;
      tree_widget.header()->close();
      tree_widget.setContextMenuPolicy(Qt::CustomContextMenu);
      connect(
        &tree_widget,
        SIGNAL(customContextMenuRequested(const QPoint &)),
        SLOT(prepareMenu(const QPoint &))
      );
      QTreeWidgetItem &charmapper_item = addItemTo(tree_widget,"charmapper");
      charmapper_item_ptr = &charmapper_item;
    }
    {
      QPushButton &button = addPushButtonTo(layout2);
      button.setText("test");
    }
  }
  addDiagramEditorTo(layout,diagram);
  setCentralWidget(&widget);
}


void QtMainWindow::prepareMenu(const QPoint &pos)
{
  cerr << "prepareMenu()\n";
  QTreeWidgetItem *widget_item_ptr = treeWidget().itemAt(pos);

  if (!widget_item_ptr) {
    return;
  }

  if (widget_item_ptr==charmapper_item_ptr) {
    QMenu menu;
    QAction &add_pass_action = addActionTo(menu,"Add Motion Pass");
    connect(&add_pass_action,SIGNAL(triggered()),SLOT(addPassTriggered()));
    menu.exec(treeWidget().mapToGlobal(pos));
    return;
  }

  if (widget_item_ptr==motion_pass_item_ptr) {
    QMenu menu;
    QAction &add_pos_expr_action = addActionTo(menu,"Add Pos Expr");
    connect(
      &add_pos_expr_action,SIGNAL(triggered()),SLOT(addPosExprTriggered())
    );
    menu.exec(treeWidget().mapToGlobal(pos));
    return;
  }
}


void QtMainWindow::addPassTriggered()
{
  assert(charmapper_item_ptr);

  motion_pass_item_ptr = &addChildItemTo(*charmapper_item_ptr,"Motion Pass");
}


void QtMainWindow::addPosExprTriggered()
{
  assert(motion_pass_item_ptr);

  QTreeWidgetItem &item = addChildItemTo(*motion_pass_item_ptr,"Pos Expr");
  add_pos_expr_item_ptr = &item;
  addChildItemTo(item,"Target Body");
  addChildItemTo(item,"Local Position");
  addChildItemTo(item,"Global Position");
  addChildItemTo(item,"Weight");
}


QTreeWidget &QtMainWindow::treeWidget()
{
  assert(tree_widget_ptr);
  return *tree_widget_ptr;
}
