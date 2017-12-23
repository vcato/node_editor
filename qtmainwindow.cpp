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
#include "qttreewidget.hpp"
#include "qtwidget.hpp"


using std::cerr;
using std::string;
using std::vector;
using std::ostream;
using TreePath = Tree::Path;



template <typename Layout>
static Layout& createLayout(QBoxLayout &parent_layout)
{
  Layout *layout_ptr = new Layout;
  parent_layout.addLayout(layout_ptr);
  return *layout_ptr;
}


static QtTreeWidget& createTreeWidget(QLayout &layout)
{
  return createWidget<QtTreeWidget>(layout);
}


static QAction& createAction(QMenu &menu,const string &label)
{
  QAction *add_pass_action_ptr = new QAction(QString::fromStdString(label),0);
  menu.addAction(add_pass_action_ptr);
  return *add_pass_action_ptr;
}


static QtDiagramEditor&
  createDiagramEditor(QBoxLayout &layout,int stretch,Diagram &diagram)
{
  QtDiagramEditor *diagram_editor_ptr = new QtDiagramEditor(diagram);
  layout.addWidget(diagram_editor_ptr,stretch);
  return *diagram_editor_ptr;
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


static QTreeWidgetItem&
  createItem(QTreeWidgetItem &parent_item,const string &label)
{
  QTreeWidgetItem &pass_item = QtTreeWidget::createItem(parent_item);
  setItemText(pass_item,label);
  return pass_item;
}


void QtMainWindow::createTree(QBoxLayout &parent_layout)
{
  QtTreeWidget &tree_widget = createTreeWidget(parent_layout);
  tree_widget_ptr = &tree_widget;
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
  tree.createCharmapperItem();
  createItem(tree_widget,"charmapper");
}


static QTreeWidgetItem &
  itemFromPath(const QTreeWidget &tree_widget,const vector<int> &path)
{
  int path_length = path.size();
  QTreeWidgetItem *item_ptr = tree_widget.topLevelItem(path[0]);
  assert(path_length>0);

  int i = 1;
  while (i!=path_length) {
    item_ptr = item_ptr->child(path[i]);
    ++i;
  }

  assert(item_ptr);

  return *item_ptr;
}


QtMainWindow::QtMainWindow()
: tree_widget_ptr(0),
  diagram_editor_ptr(0)
{
  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  menu_bar_ptr->addMenu(&menu);

  QHBoxLayout &layout = createLayout<QHBoxLayout>(widget);
  createTree(layout);
  diagram_editor_ptr = &createDiagramEditor(layout,/*stretch*/1,diagram);
  setCentralWidget(&widget);
  assert(tree_widget_ptr);
  itemFromPath(*tree_widget_ptr,{0}).setSelected(true);
}


static void buildPath(vector<int> &path,QTreeWidgetItem &item)
{
  if (!item.parent()) {
    path.push_back(0);
    return;
  }
  buildPath(path,*item.parent());
  path.push_back(item.parent()->indexOfChild(&item));
}


static vector<int> itemPath(QTreeWidgetItem &item)
{
  vector<int> path;
  buildPath(path,item);
  return path;
}


template <typename T>
static ostream& operator<<(ostream &stream,const vector<T> &value)
{
  stream << "[";

  if (!value.empty()) {
    auto iter = value.begin();

    stream << *iter++;

    while (iter!=value.end()) {
      stream << ",";
      stream << *iter++;
    }
  }

  stream << "]";

  return stream;
}


void QtMainWindow::prepareMenu(const QPoint &pos)
{
  QTreeWidgetItem *widget_item_ptr = treeWidget().itemAt(pos);

  if (!widget_item_ptr) {
    return;
  }

  Tree::Path path = itemPath(*widget_item_ptr);

  bool is_charmapper_item = tree.isCharmapperItem(path);

  if (is_charmapper_item) {
    QMenu menu;
    QAction &add_pass_action = createAction(menu,"Add Motion Pass");
    connect(&add_pass_action,SIGNAL(triggered()),SLOT(addPassTriggered()));
    menu.exec(treeWidget().mapToGlobal(pos));
    return;
  }

  bool is_motion_pass_item = tree.isMotionPassItem(path);

  if (is_motion_pass_item) {
    QMenu menu;
    QAction &add_pos_expr_action = createAction(menu,"Add Pos Expr");
    connect(
      &add_pos_expr_action,SIGNAL(triggered()),SLOT(addPosExprTriggered())
    );
    menu.exec(treeWidget().mapToGlobal(pos));
    return;
  }
}


void QtMainWindow::treeItemSelectionChanged()
{
  QTreeWidgetItem *selected_item_ptr = findSelectedItem();
  Diagram *diagram_ptr = 0;
  if (selected_item_ptr) {
    diagram_ptr = &tree.itemDiagram(itemPath(*selected_item_ptr));
  }
  assert(diagram_editor_ptr);
  QtDiagramEditor &diagram_editor = *diagram_editor_ptr;
  diagram_editor.setDiagramPtr(diagram_ptr);
}


QTreeWidgetItem* QtMainWindow::findSelectedItem()
{
  assert(tree_widget_ptr);
  QList<QTreeWidgetItem*> items = tree_widget_ptr->selectedItems();

  if (items.size()!=1) {
    return nullptr;
  }

  return items.front();
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
    tree.createMotionPassItem(selected_item_path);

  cerr << "new item index: " << motion_pass_item_path << "\n";
  assert(tree.isMotionPassItem(motion_pass_item_path));
  createItem(*selected_item_ptr,"Motion Pass");
}


void
  QtMainWindow::addTreeItem(
    const TreePath &parent_path,
    const TreeItem &item
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(treeWidget(),parent_path);

  switch (item.type) {
    case TreeItem::Type::x:
      tree.createXItem(parent_path);
      treeWidget().createSpinBoxItem(parent_item,"X");
      break;
    case TreeItem::Type::y:
      tree.createXItem(parent_path);
      treeWidget().createSpinBoxItem(parent_item,"Y");
      break;
    case TreeItem::Type::z:
      tree.createXItem(parent_path);
      treeWidget().createSpinBoxItem(parent_item,"Y");
      break;
    case TreeItem::Type::global_position:
      {
        TreePath global_position_path =
          tree.createGlobalPositionItem(parent_path);
        QtComboBoxTreeWidgetItem &global_position_item =
          treeWidget().createComboBoxItem(parent_item,"Global Position");
        {
          QComboBox &combo_box = global_position_item.comboBox();
          combo_box.addItem("Components");
          combo_box.addItem("From Body");
        }
        addTreeItems(global_position_path,item);
      }
      break;
    case TreeItem::Type::local_position:
      {
        TreePath local_position_path =
          tree.createLocalPositionItem(parent_path);
        createItem(parent_item,"Local Position");
        addTreeItems(local_position_path,item);
      }
      break;
    case TreeItem::Type::target_body:
      {
        tree.createTargetBodyItem(parent_path);
        QtComboBoxTreeWidgetItem &target_body_item =
          treeWidget().createComboBoxItem(parent_item,"Target Body");
        QComboBox &combo_box = target_body_item.comboBox();
        combo_box.addItem("Body1");
        combo_box.addItem("Body2");
        combo_box.addItem("Body3");
      }
      break;
    case TreeItem::Type::pos_expr:
      {
        TreePath pos_expr_path = tree.createPosExprItem(parent_path);
        createItem(parent_item,"Pos Expr");
        addTreeItems(pos_expr_path,item);
      }
      break;
    default:
      assert(false);
  }
}


void
  QtMainWindow::addTreeItems(
    const TreePath &parent_path,
    const TreeItem &tree_items
  )
{
  for (const TreeItem &item : tree_items.child_items) {
    addTreeItem(parent_path,item);
  }
}


static void createXYZChildren(TreeItem &parent_item)
{
  using ItemType = TreeItem::Type;

  parent_item.createItem2(ItemType::x);
  parent_item.createItem2(ItemType::y);
  parent_item.createItem2(ItemType::z);
}


void QtMainWindow::addPosExprTriggered()
{
  QTreeWidgetItem *parent_item_ptr = findSelectedItem();
  assert(parent_item_ptr);
  using TreePath = Tree::Path;
  using ItemType = TreeItem::Type;

  TreePath parent_path = itemPath(*parent_item_ptr);
  TreeItem pos_expr_item(ItemType::pos_expr);
  pos_expr_item.createItem2(ItemType::target_body);
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(ItemType::local_position);
    createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(ItemType::global_position);
    createXYZChildren(global_position_item);
  }
  addTreeItem(parent_path,pos_expr_item);
}


void
  QtMainWindow::treeComboBoxItemIndexChanged(
    QtComboBoxTreeWidgetItem *item_ptr,
    int
  )
{
  assert(item_ptr);
}


QtTreeWidget &QtMainWindow::treeWidget()
{
  assert(tree_widget_ptr);
  return *tree_widget_ptr;
}
