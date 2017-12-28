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
#include "qttreewidget.hpp"
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


static QtTreeWidget& createTreeWidget(QSplitter &parent_splitter)
{
  return createWidget<QtTreeWidget>(parent_splitter);
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
  QtTreeWidget &tree_widget = createTreeWidget(parent_splitter);
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
  tree_widget.createItem("charmapper");
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


template <typename T>
static T& createCentralWidget(QMainWindow &parent)
{
  T *widget_ptr = new T;
  parent.setCentralWidget(widget_ptr);
  T &widget = *widget_ptr;
  return widget;
}


QtMainWindow::QtMainWindow()
: tree_widget_ptr(0),
  diagram_editor_ptr(0)
{
  QMenuBar *menu_bar_ptr = menuBar();
  assert(menu_bar_ptr);
  menu_bar_ptr->addMenu(&menu);

  QSplitter &splitter = createCentralWidget<QSplitter>(*this);
  createTree(splitter);
  diagram_editor_ptr = &createDiagramEditor(splitter,/*stretch*/1);
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


Diagram *QtMainWindow::selectedDiagramPtr()
{
  QTreeWidgetItem *selected_item_ptr = findSelectedItem();
  Diagram *diagram_ptr = 0;

  if (selected_item_ptr) {
    diagram_ptr = &tree.itemDiagram(itemPath(*selected_item_ptr));
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

  assert(tree.isMotionPassItem(motion_pass_item_path));
  treeWidget().createItem(*selected_item_ptr,"Motion Pass");
}


void
  QtMainWindow::addTreeItem(
    const TreePath &parent_path,
    const TreeItem &item
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(treeWidget(),parent_path);
  TreePath new_item_path;

  switch (item.type) {
    case TreeItem::Type::x:
      new_item_path = tree.createXItem(parent_path);
      treeWidget().createSpinBoxItem(parent_item,"X");
      break;
    case TreeItem::Type::y:
      new_item_path = tree.createXItem(parent_path);
      treeWidget().createSpinBoxItem(parent_item,"Y");
      break;
    case TreeItem::Type::z:
      new_item_path = tree.createXItem(parent_path);
      treeWidget().createSpinBoxItem(parent_item,"Y");
      break;
    case TreeItem::Type::global_position:
      {
        new_item_path =
          tree.createGlobalPositionItem(parent_path);
        QtComboBoxTreeWidgetItem &global_position_item =
          treeWidget().createComboBoxItem(parent_item,"Global Position");
        {
          QComboBox &combo_box = global_position_item.comboBox();
          ignore_combo_box_signals = true;
          combo_box.addItem("Components");
          combo_box.addItem("From Body");
          ignore_combo_box_signals = false;
        }
      }
      break;
    case TreeItem::Type::local_position:
      {
        new_item_path = tree.createLocalPositionItem(parent_path);
        treeWidget().createItem(parent_item,"Local Position");
      }
      break;
    case TreeItem::Type::target_body:
      {
        new_item_path = tree.createTargetBodyItem(parent_path);
        QtComboBoxTreeWidgetItem &target_body_item =
          treeWidget().createComboBoxItem(parent_item,"Target Body");
        QComboBox &combo_box = target_body_item.comboBox();
        combo_box.addItem("Body1");
        combo_box.addItem("Body2");
        combo_box.addItem("Body3");
      }
      break;
    case TreeItem::Type::source_body:
      {
        new_item_path = tree.createSourceBodyItem(parent_path);
        QtComboBoxTreeWidgetItem &source_body_item =
          treeWidget().createComboBoxItem(parent_item,"Source Body");
        QComboBox &combo_box = source_body_item.comboBox();
        combo_box.addItem("Body1");
        combo_box.addItem("Body2");
        combo_box.addItem("Body3");
      }
      break;
    case TreeItem::Type::pos_expr:
      {
        new_item_path = tree.createPosExprItem(parent_path);
        treeWidget().createItem(parent_item,"Pos Expr");
      }
      break;
    default:
      assert(false);
  }

  tree.itemDiagram(new_item_path) = item.diagram;
  addTreeItems(new_item_path,item);
}


void QtMainWindow::removeChildItems(const TreePath &path)
{
  tree.removeChildItems(path);

  QTreeWidgetItem &item = itemFromPath(treeWidget(),path);

  while (item.childCount()>0) {
    item.removeChild(item.child(item.childCount()-1));
  }
}


void
  QtMainWindow::replaceTreeItems(
    const TreePath &parent_path,
    const TreeItem &tree_items
  )
{
  removeChildItems(parent_path);
  addTreeItems(parent_path,tree_items);
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


static Diagram makeDiagram(const char *text)
{
  istringstream stream(text);
  Diagram diagram;
  scanDiagramFrom(stream,diagram);
  return diagram;
}


static Diagram posExprDiagram()
{
  const char *text = R"text(
diagram {
  node {
    id: 4
    position: [485,153]
    text {
      "$-$"
    }
    connection {
      input_index: 0
      source_node_id: 5
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 7
      source_output_index: 0
    }
  }
  node {
    id: 5
    position: [266,183]
    text {
      "global_position"
    }
  }
  node {
    id: 7
    position: [274,108]
    text {
      "$.globalVec($)"
    }
    connection {
      input_index: 0
      source_node_id: 8
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 8
      source_output_index: 1
    }
  }
  node {
    id: 8
    position: [59,158]
    text {
      "target_body"
      "local_position"
    }
  }
  node {
    id: 10
    position: [559,233]
    text {
      "$.pos=$"
    }
    connection {
      input_index: 0
      source_node_id: 13
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 4
      source_output_index: 0
    }
  }
  node {
    id: 13
    position: [283,233]
    text {
      "$"
    }
    connection {
      input_index: 0
      source_node_id: 8
      source_output_index: 0
    }
  }
}
)text";

  return makeDiagram(text);
}


static Diagram fromComponentsDiagram()
{
  const char *text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position: [71,280]\n"
    "    text {\n"
    "      \"x\"\n"
    "      \"y\"\n"
    "      \"z\"\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 2\n"
    "    position: [183,280]\n"
    "    text {\n"
    "      \"[$,$,$]\"\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 1\n"
    "      source_output_index: 0\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 1\n"
    "      source_node_id: 1\n"
    "      source_output_index: 1\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 2\n"
    "      source_node_id: 1\n"
    "      source_output_index: 2\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 4\n"
    "    position: [331,249]\n"
    "    text {\n"
    "      \"return $\"\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 2\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "}\n";

  return makeDiagram(text);
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
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(ItemType::local_position);
    createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(ItemType::global_position);
    createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }
  addTreeItem(parent_path,pos_expr_item);
}


static Diagram fromBodyDiagram()
{
  const char *text = R"text(
diagram {
  node {
    id: 1
    position: [263,328]
    text {
      "$.pos($)"
    }
    connection {
      input_index: 0
      source_node_id: 5
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 5
      source_output_index: 1
    }
  }
  node {
    id: 4
    position: [410,313]
    text {
      "return $"
    }
    connection {
      input_index: 0
      source_node_id: 1
      source_output_index: 0
    }
  }
  node {
    id: 5
    position: [50,328]
    text {
      "source_body"
      "local_position"
    }
  }
}
)text";

  return makeDiagram(text);
}


void
  QtMainWindow::treeComboBoxItemIndexChanged(
    QtComboBoxTreeWidgetItem *item_ptr,
    int index
  )
{
  if (ignore_combo_box_signals) return;

  assert(item_ptr);

  Tree::Path path = itemPath(*item_ptr);

  if (tree.isGlobalPositionItem(path)) {
    switch (index) {
      case 0:
        // Components
        {
          TreeItem items(TreeItem::Type::root);
          createXYZChildren(items);
          replaceTreeItems(path,items);
          tree.itemDiagram(path) = fromComponentsDiagram();
          diagramEditor().redraw();
        }
        break;
      case 1:
        // From Body
        {
          TreeItem items(TreeItem::Type::root);
          items.createItem(TreeItem::Type::source_body);
          TreeItem &local_position_item =
            items.createItem2(TreeItem::Type::local_position);
          createXYZChildren(local_position_item);
          replaceTreeItems(path,items);
          tree.itemDiagram(path) = fromBodyDiagram();
          diagramEditor().redraw();
        }
        break;
      default:
        assert(false);
    }
  }

}


QtTreeWidget &QtMainWindow::treeWidget()
{
  assert(tree_widget_ptr);
  return *tree_widget_ptr;
}
