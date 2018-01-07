#include "qtmainwindow.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <QMenuBar>
#include <QBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QSplitter>
#include "qttreeeditor.hpp"
#include "qtwidget.hpp"
#include "qtmenu.hpp"
#include "diagramio.hpp"


using std::cerr;
using std::string;
using std::vector;
using std::ostream;
using std::istringstream;
using TreePath = Tree::Path;



template <typename Layout>
static Layout& createLayout(QBoxLayout &parent_layout)
{
  Layout *layout_ptr = new Layout;
  parent_layout.addLayout(layout_ptr);
  return *layout_ptr;
}


static QtTreeEditor& createTreeEditor(QSplitter &parent_splitter)
{
  return createWidget<QtTreeEditor>(parent_splitter);
}


static QtDiagramEditor& createDiagramEditor(QSplitter &splitter,int stretch)
{
  int index = splitter.count();
  QtDiagramEditor *diagram_editor_ptr = new QtDiagramEditor;
  splitter.addWidget(diagram_editor_ptr);
  splitter.setStretchFactor(index,stretch);
  return *diagram_editor_ptr;
}


void QtMainWindow::createTree(QSplitter &parent_splitter)
{
  QtTreeEditor &tree_widget = createTreeEditor(parent_splitter);
  tree_editor_ptr = &tree_widget;
  tree_widget.setTreePtr(&tree());
  tree_widget.header()->close();
  tree_widget.setContextMenuPolicy(Qt::CustomContextMenu);
  connect(
    &tree_widget,
    SIGNAL(customContextMenuRequested(const QPoint &)),
    SLOT(prepareMenu(const QPoint &))
  );
  connect(
    &tree_widget,
    SIGNAL(itemSelectionChanged()),
    SLOT(treeItemSelectionChanged())
  );
  connect(
    &tree_widget,
    SIGNAL(comboBoxItemIndexChanged(QtComboBoxTreeWidgetItem*,int)),
    SLOT(treeComboBoxItemIndexChanged(QtComboBoxTreeWidgetItem*,int))
  );
  tree().createCharmapperItem();
  tree_widget.createItem("charmapper");
}


template <typename T>
static T& createCentralWidget(QMainWindow &parent)
{
  T *widget_ptr = new T;
  parent.setCentralWidget(widget_ptr);
  T &widget = *widget_ptr;
  return widget;
}


QtMainWindow::QtMainWindow()
: tree_editor_ptr(0),
  diagram_editor_ptr(0)
{
  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  menu_bar_ptr->addMenu(&menu);

  QSplitter &splitter = createCentralWidget<QSplitter>(*this);
  createTree(splitter);
  diagram_editor_ptr = &createDiagramEditor(splitter,/*stretch*/1);
  treeEditor().setDiagramEditorPtr(diagram_editor_ptr);
  treeEditor().itemFromPath({0}).setSelected(true);
}


vector<int> QtMainWindow::itemPath(QTreeWidgetItem &item)
{
  return treeEditor().itemPath(item);
}


void QtMainWindow::prepareMenu(const QPoint &pos)
{
  QtTreeEditor &tree_editor = treeEditor();
  QTreeWidgetItem *widget_item_ptr = tree_editor.itemAt(pos);

  if (!widget_item_ptr) {
    return;
  }

  Tree::Path path = itemPath(*widget_item_ptr);

  bool is_charmapper_item = tree().isCharmapperItem(path);

  if (is_charmapper_item) {
    QMenu menu;
    QAction &add_pass_action = createAction(menu,"Add Motion Pass");
    connect(&add_pass_action,SIGNAL(triggered()),SLOT(addPassTriggered()));
    menu.exec(tree_editor.mapToGlobal(pos));
    return;
  }

  bool is_motion_pass_item = tree().isMotionPassItem(path);

  if (is_motion_pass_item) {
    QMenu menu;
    QAction &add_pos_expr_action = createAction(menu,"Add Pos Expr");
    connect(
      &add_pos_expr_action,SIGNAL(triggered()),SLOT(addPosExprTriggered())
    );
    menu.exec(tree_editor.mapToGlobal(pos));
    return;
  }
}


Diagram *QtMainWindow::selectedDiagramPtr()
{
  QTreeWidgetItem *selected_item_ptr = findSelectedItem();
  Diagram *diagram_ptr = 0;

  if (selected_item_ptr) {
    diagram_ptr = &tree().itemDiagram(itemPath(*selected_item_ptr));
  }

  return diagram_ptr;
}


QtDiagramEditor &QtMainWindow::diagramEditor()
{
  assert(diagram_editor_ptr);
  return *diagram_editor_ptr;
}


void QtMainWindow::treeItemSelectionChanged()
{
  diagramEditor().setDiagramPtr(selectedDiagramPtr());
}


QTreeWidgetItem* QtMainWindow::findSelectedItem()
{
  return treeEditor().findSelectedItem();
}


void QtMainWindow::addPassTriggered()
{
  QTreeWidgetItem *selected_item_ptr = findSelectedItem();

  if (!selected_item_ptr) {
    cerr << "addPassTriggered: No selected item!\n";
    return;
  }

  Tree::Path selected_item_path = itemPath(*selected_item_ptr);

  Tree::Path motion_pass_item_path =
    tree().createMotionPassItem(selected_item_path);

  assert(tree().isMotionPassItem(motion_pass_item_path));
  treeEditor().createItem(*selected_item_ptr,"Motion Pass");
}


void
  QtMainWindow::addTreeItem(
    const TreePath &parent_path,
    const TreeItem &item
  )
{
  treeEditor().addTreeItem(parent_path,item);
}


void
  QtMainWindow::addTreeItems(
    const TreePath &parent_path,
    const TreeItem &tree_items
  )
{
  treeEditor().addTreeItems(parent_path,tree_items);
}


void QtMainWindow::addPosExprTriggered()
{
  treeEditor().handleAddPosExpr();
}


void
  QtMainWindow::treeComboBoxItemIndexChanged(
    QtComboBoxTreeWidgetItem *item_ptr,
    int index
  )
{
  treeEditor().handleComboBoxItemIndexChanged(item_ptr,index);
}


QtTreeEditor &QtMainWindow::treeEditor()
{
  assert(tree_editor_ptr);
  return *tree_editor_ptr;
}
