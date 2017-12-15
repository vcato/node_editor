#include "qtmainwindow.hpp"

#include <cassert>
#include <iostream>
#include <QMenuBar>
#include <QBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>


using std::cerr;
using std::string;


template <typename Layout>
static Layout& createLayout(QWidget &widget)
{
  Layout *layout_ptr = new Layout;
  widget.setLayout(layout_ptr);
  return *layout_ptr;
}


template <typename Layout>
static Layout& createLayout(QBoxLayout &parent_layout)
{
  Layout *layout_ptr = new Layout;
  parent_layout.addLayout(layout_ptr);
  return *layout_ptr;
}


template <typename Widget>
static Widget& createWidget(QLayout &layout,Widget *widget_ptr)
{
  layout.addWidget(widget_ptr);
  return *widget_ptr;
}


static QTreeWidget& createTreeWidget(QLayout &layout)
{
  return createWidget(layout,new QTreeWidget);
}


static QPushButton& createPushButton(QLayout &layout)
{
  return createWidget(layout,new QPushButton);
}


static QAction& createAction(QMenu &menu,const string &label)
{
  QAction *add_pass_action_ptr = new QAction(QString::fromStdString(label),0);
  menu.addAction(add_pass_action_ptr);
  return *add_pass_action_ptr;
}


static void createDiagramEditor(QLayout &layout,Diagram &diagram)
{
  QtDiagramEditor *diagram_editor_ptr = new QtDiagramEditor(diagram);
  layout.addWidget(diagram_editor_ptr);
}


static void setItemText(QTreeWidgetItem &item,const string &label)
{
  item.setText(/*column*/0,QString::fromStdString(label));
}


static QTreeWidgetItem& createItem(QTreeWidget &tree_widget,const string &label)
{
  QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
  setItemText(*item_ptr,label);
  tree_widget.addTopLevelItem(item_ptr);
  item_ptr->setExpanded(true);
  return *item_ptr;
}


static QTreeWidgetItem& createItem(QTreeWidgetItem &parent_item)
{
  QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
  parent_item.addChild(item_ptr);
  QTreeWidgetItem &item = *item_ptr;
  item.setExpanded(true);
  return item;
}


static QTreeWidgetItem&
  createItem(QTreeWidgetItem &parent_item,const string &label)
{
  QTreeWidgetItem &pass_item = createItem(parent_item);
  setItemText(pass_item,label);
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

  QHBoxLayout &layout = createLayout<QHBoxLayout>(widget);
  {
    QVBoxLayout &layout2 = createLayout<QVBoxLayout>(layout);

    {
      QTreeWidget &tree_widget = createTreeWidget(layout2);
      tree_widget_ptr = &tree_widget;
      tree_widget.header()->close();
      tree_widget.setContextMenuPolicy(Qt::CustomContextMenu);
      connect(
        &tree_widget,
        SIGNAL(customContextMenuRequested(const QPoint &)),
        SLOT(prepareMenu(const QPoint &))
      );
      QTreeWidgetItem &charmapper_item = createItem(tree_widget,"charmapper");
      charmapper_item_ptr = &charmapper_item;
    }
    {
      QPushButton &button = createPushButton(layout2);
      button.setText("test");
    }
  }
  createDiagramEditor(layout,diagram);
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
    QAction &add_pass_action = createAction(menu,"Add Motion Pass");
    connect(&add_pass_action,SIGNAL(triggered()),SLOT(addPassTriggered()));
    menu.exec(treeWidget().mapToGlobal(pos));
    return;
  }

  if (widget_item_ptr==motion_pass_item_ptr) {
    QMenu menu;
    QAction &add_pos_expr_action = createAction(menu,"Add Pos Expr");
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

  motion_pass_item_ptr = &createItem(*charmapper_item_ptr,"Motion Pass");
}


template <typename T>
static T &
  setItemWidget(
    QTreeWidget &tree_widget,
    QTreeWidgetItem &test_item,
    const std::string &label
  )
{
  QWidget *wrapper_widget_ptr = new QWidget();
  QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);
  createWidget(layout,new QLabel(QString::fromStdString(label)));
  T* widget_ptr = new T();
  createWidget(layout,widget_ptr);
  tree_widget.setItemWidget(&test_item,/*column*/0,wrapper_widget_ptr);
  return *widget_ptr;
}


static void
  createItemSpinBox(
    QTreeWidget &tree_widget,
    QTreeWidgetItem &local_position_item,
    const string &label
  )
{
  QTreeWidgetItem &x_item = createItem(local_position_item);
  setItemWidget<QSpinBox>(tree_widget,x_item,label);
}


void QtMainWindow::addPosExprTriggered()
{
  assert(motion_pass_item_ptr);

  QTreeWidgetItem &item = createItem(*motion_pass_item_ptr,"Pos Expr");
  add_pos_expr_item_ptr = &item;
  {
    QTreeWidgetItem &test_item = createItem(item);
    QComboBox &combo_box =
      setItemWidget<QComboBox>(treeWidget(),test_item,"Target Body");
    combo_box.addItem("Body1");
    combo_box.addItem("Body2");
    combo_box.addItem("Body3");
  }
  QTreeWidgetItem &local_position_item = createItem(item,"Local Position");
  {
    createItemSpinBox(treeWidget(),local_position_item,"X");
    createItemSpinBox(treeWidget(),local_position_item,"Y");
    createItemSpinBox(treeWidget(),local_position_item,"Z");
  }
  QTreeWidgetItem &global_position_item = createItem(item,"Global Position");
  {
    createItemSpinBox(treeWidget(),global_position_item,"X");
    createItemSpinBox(treeWidget(),global_position_item,"Y");
    createItemSpinBox(treeWidget(),global_position_item,"Z");
  }
  createItemSpinBox(treeWidget(),item,"Weight");
}


QTreeWidget &QtMainWindow::treeWidget()
{
  assert(tree_widget_ptr);
  return *tree_widget_ptr;
}
