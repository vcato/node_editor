#include "qttreeeditor.hpp"

#include <iostream>
#include <sstream>
#include <QMenu>
#include <QHeaderView>
#include "diagramio.hpp"
#include "qtmenu.hpp"
#include "qtslot.hpp"

using std::cerr;
using std::vector;
using std::istringstream;
using std::ostream;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::function;
using std::list;


QtTreeEditor::QtTreeEditor()
: operation_handler(*this)
{
  header()->close();
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(
    this,
    SIGNAL(itemSelectionChanged()),
    SLOT(itemSelectionChangedSlot())
  );
  connect(
    this,
    SIGNAL(customContextMenuRequested(const QPoint &)),
    SLOT(prepareMenuSlot(const QPoint &))
  );
}


void
  QtTreeEditor::comboBoxItemCurrentIndexChangedSlot(
    QtComboBoxTreeWidgetItem *item_ptr,
    int index
  )
{
  handleComboBoxItemIndexChanged(item_ptr,index);
}


QTreeWidgetItem& QtTreeEditor::createItem(const std::string &label)
{
  QTreeWidgetItem &item = createItem();
  setItemText(item,label);
  return item;
}


QTreeWidgetItem&
  QtTreeEditor::createItem(
    QTreeWidgetItem &parent_item,
    const std::string &label
  )
{
  QTreeWidgetItem &pass_item = QtTreeEditor::createItem(parent_item);
  setItemText(pass_item,label);
  return pass_item;
}


QTreeWidgetItem& QtTreeEditor::createItem(QTreeWidgetItem &parent_item)
{
  QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
  parent_item.addChild(item_ptr);
  QTreeWidgetItem &item = *item_ptr;
  item.setExpanded(true);
  return item;
}


QTreeWidgetItem& QtTreeEditor::createItem()
{
  QTreeWidget &tree_widget = *this;
  QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
  tree_widget.addTopLevelItem(item_ptr);
  item_ptr->setExpanded(true);
  return *item_ptr;
}


void QtTreeEditor::setItemText(QTreeWidgetItem &item,const std::string &label)
{
  item.setText(/*column*/0,QString::fromStdString(label));
}


QtComboBoxTreeWidgetItem&
  QtTreeEditor::createComboBoxItem(
    QTreeWidgetItem &parent_item,
    const std::string &label
  )
{
  QtComboBoxTreeWidgetItem *item_ptr = new QtComboBoxTreeWidgetItem;
  parent_item.addChild(item_ptr);
  QtComboBoxTreeWidgetItem &item = *item_ptr;
  item.setExpanded(true);
  QComboBox &combo_box = createItemWidget<QComboBox>(item,label);
  item.combo_box_ptr = &combo_box;
  item.signal_map.connect(
    &combo_box,
    SIGNAL(currentIndexChanged(int)),
    SLOT(currentIndexChangedSlot(int))
  );
  connect(
    &item.signal_map,
    SIGNAL(currentIndexChanged(QtComboBoxTreeWidgetItem*,int)),
    SLOT(comboBoxItemCurrentIndexChangedSlot(QtComboBoxTreeWidgetItem*,int))
  );
  return item;
}


void
  QtTreeEditor::createSpinBoxItem(
    QTreeWidgetItem &parent_item,
    const std::string &label
  )
{
  QtTreeEditor &tree_widget = *this;
  QTreeWidgetItem &item = createItem(parent_item);
  tree_widget.createItemWidget<QSpinBox>(item,label);
}


void
  QtTreeEditor::addTreeItem(
    const TreePath &parent_path,
    const TreeItem &item
  )
{
  QtTreeEditor &tree_widget = *this;
  Tree &tree = this->tree();
  QTreeWidgetItem &parent_item = tree_widget.itemFromPath(parent_path);
  TreePath new_item_path;

  switch (item.type) {
    case TreeItem::Type::x:
      new_item_path = tree.createXItem(parent_path);
      tree_widget.createSpinBoxItem(parent_item,"X");
      break;
    case TreeItem::Type::y:
      new_item_path = tree.createXItem(parent_path);
      tree_widget.createSpinBoxItem(parent_item,"Y");
      break;
    case TreeItem::Type::z:
      new_item_path = tree.createXItem(parent_path);
      tree_widget.createSpinBoxItem(parent_item,"Y");
      break;
    case TreeItem::Type::global_position:
      {
        new_item_path =
          tree.createGlobalPositionItem(parent_path);
        QtComboBoxTreeWidgetItem &global_position_item =
          tree_widget.createComboBoxItem(parent_item,"Global Position");
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
        tree_widget.createItem(parent_item,"Local Position");
      }
      break;
    case TreeItem::Type::target_body:
      {
        new_item_path = tree.createTargetBodyItem(parent_path);
        QtComboBoxTreeWidgetItem &target_body_item =
          tree_widget.createComboBoxItem(parent_item,"Target Body");
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
          tree_widget.createComboBoxItem(parent_item,"Source Body");
        QComboBox &combo_box = source_body_item.comboBox();
        combo_box.addItem("Body1");
        combo_box.addItem("Body2");
        combo_box.addItem("Body3");
      }
      break;
    case TreeItem::Type::pos_expr:
      {
        new_item_path = tree.createPosExprItem(parent_path);
        tree_widget.createItem(parent_item,"Pos Expr");
      }
      break;
    default:
      assert(false);
  }

  tree.itemDiagram(new_item_path) = item.diagram;
  addTreeItems(new_item_path,item);
}


QTreeWidgetItem &QtTreeEditor::itemFromPath(const std::vector<int> &path) const
{
  const QtTreeEditor &tree_widget = *this;
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


Tree &QtTreeEditor::tree()
{
  assert(tree_ptr);
  return *tree_ptr;
}


void
  QtTreeEditor::addTreeItems(
    const TreePath &parent_path,
    const TreeItem &tree_items
  )
{
  for (const TreeItem &item : tree_items.child_items) {
    addTreeItem(parent_path,item);
  }
}


void QtTreeEditor::buildPath(vector<int> &path,QTreeWidgetItem &item)
{
  QTreeWidgetItem *parent_item_ptr = item.parent();

  if (!parent_item_ptr) {
    path.push_back(indexOfTopLevelItem(&item));
    return;
  }

  buildPath(path,*parent_item_ptr);
  path.push_back(parent_item_ptr->indexOfChild(&item));
}


std::vector<int> QtTreeEditor::itemPath(QTreeWidgetItem &item)
{
  vector<int> path;
  buildPath(path,item);
  return path;
}


static Diagram makeDiagram(const char *text)
{
  istringstream stream(text);
  Diagram diagram;
  scanDiagramFrom(stream,diagram);
  return diagram;
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


void QtTreeEditor::removeChildItems(const TreePath &path)
{
  tree().removeChildItems(path);

  QTreeWidgetItem &item = treeEditor().itemFromPath(path);

  while (item.childCount()>0) {
    item.removeChild(item.child(item.childCount()-1));
  }
}


void
  QtTreeEditor::replaceTreeItems(
    const TreePath &parent_path,
    const TreeItem &tree_items
  )
{
  removeChildItems(parent_path);
  addTreeItems(parent_path,tree_items);
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


void QtTreeEditor::handleAddPosExpr()
{
  QTreeWidgetItem *parent_item_ptr = findSelectedItem();
  assert(parent_item_ptr);
  using ItemType = TreeItem::Type;

  TreePath parent_path = itemPath(*parent_item_ptr);
  TreeItem pos_expr_item(ItemType::pos_expr);
  pos_expr_item.createItem2(ItemType::target_body);
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(ItemType::local_position);
    tree().createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(ItemType::global_position);
    tree().createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }
  addTreeItem(parent_path,pos_expr_item);
}


void QtTreeEditor::handleAddPass()
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


void QtTreeEditor::handleAddScene()
{
  tree().createSceneItem();
  treeEditor().createItem("Scene");
}


void QtTreeEditor::handleAddCharmapper()
{
  tree().createCharmapperItem();
  treeEditor().createItem("Charmapper");
}


QTreeWidgetItem* QtTreeEditor::findSelectedItem()
{
  QList<QTreeWidgetItem*> items = treeEditor().selectedItems();

  if (items.size()!=1) {
    return nullptr;
  }

  return items.front();
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
  QtTreeEditor::handleComboBoxItemIndexChanged(
    QtComboBoxTreeWidgetItem *item_ptr,
    int index
  )
{
  if (ignore_combo_box_signals) return;

  assert(item_ptr);

  Tree::Path path = itemPath(*item_ptr);
  Tree &tree = this->tree();

  if (tree.isGlobalPositionItem(path)) {
    switch (index) {
      case 0:
        // Components
        {
          TreeItem items(TreeItem::Type::root);
          tree.createXYZChildren(items);
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
          tree.createXYZChildren(local_position_item);
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


QtDiagramEditor &QtTreeEditor::diagramEditor()
{
  assert(diagram_editor_ptr);
  return *diagram_editor_ptr;
}


Diagram *QtTreeEditor::maybeSelectedDiagram()
{
  QTreeWidgetItem *selected_item_ptr = findSelectedItem();
  Diagram *diagram_ptr = 0;

  if (selected_item_ptr) {
    diagram_ptr = &tree().itemDiagram(itemPath(*selected_item_ptr));
  }

  return diagram_ptr;
}


void QtTreeEditor::itemSelectionChangedSlot()
{
  diagramEditor().setDiagramPtr(maybeSelectedDiagram());
}


void QtTreeEditor::prepareMenu(const QPoint &pos)
{
  QtTreeEditor &tree_editor = treeEditor();
  QTreeWidgetItem *widget_item_ptr = tree_editor.itemAt(pos);

  if (!widget_item_ptr) {
    QMenu menu;
    list<QtSlot> item_slots;

    auto add_menu_item_for_operation_function =
      [&](
        const string &operation_name,
        function<void(TreeOperationHandler &)> perform_function
      )
    {
      QAction &action = createAction(menu,operation_name);
      auto perform_operation_function =
        [perform_function,this](){ perform_function(operation_handler); };
      item_slots.emplace_back(perform_operation_function);
      item_slots.back().connect(&action,SIGNAL(triggered()),SLOT(slot()));
    };

    tree().visitOperations(add_menu_item_for_operation_function);

    menu.exec(tree_editor.mapToGlobal(pos));
    return;
  }

  Tree::Path path = itemPath(*widget_item_ptr);

  if (tree().isCharmapperItem(path)) {
    QMenu menu;
    QAction &add_pass_action = createAction(menu,"Add Motion Pass");
    connect(&add_pass_action,SIGNAL(triggered()),SLOT(addPassTriggered()));
    menu.exec(tree_editor.mapToGlobal(pos));
    return;
  }

  if (tree().isMotionPassItem(path)) {
    QMenu menu;
    QAction &add_pos_expr_action = createAction(menu,"Add Pos Expr");
    connect(
      &add_pos_expr_action,SIGNAL(triggered()),SLOT(addPosExprTriggered())
    );
    menu.exec(tree_editor.mapToGlobal(pos));
    return;
  }

  if (tree().isSceneItem(path)) {
    cerr << "Scene item\n";
  }
}


void QtTreeEditor::prepareMenuSlot(const QPoint &pos)
{
  prepareMenu(pos);
}


void QtTreeEditor::addPosExprTriggered()
{
  handleAddPosExpr();
}


void QtTreeEditor::addPassTriggered()
{
  handleAddPass();
}


void QtTreeEditor::addSceneTriggered()
{
  handleAddScene();
}


void QtTreeEditor::addCharmapperTriggered()
{
  handleAddCharmapper();
}


void QtTreeEditor::selectItem(const TreePath &path)
{
  itemFromPath(path).setSelected(true);
}
