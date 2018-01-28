#include "qttreeeditor.hpp"

#include <iostream>
#include <sstream>
#include <QMenu>
#include <QHeaderView>
#include "diagramio.hpp"
#include "qtmenu.hpp"
#include "qtslot.hpp"
#include "defaultdiagrams.hpp"


using std::cerr;
using std::vector;
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
    case TreeItem::Type::motion_pass:
      {
        new_item_path = tree.createMotionPassItem(parent_path);
        tree_widget.createItem(parent_item,"Motion Pass");
      }
      break;
    case TreeItem::Type::scene:
      {
        new_item_path = tree.createSceneItem(parent_path);
        tree_widget.createItem(parent_item,"Scene");
      }
      break;
    case TreeItem::Type::charmapper:
      {
        new_item_path = tree.createCharmapperItem(parent_path);
        tree_widget.createItem(parent_item,"Charmapper");
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

  if (path_length==0) {
    assert(tree_widget.invisibleRootItem());
    return *tree_widget.invisibleRootItem();
  }

  assert(path_length>0);
  QTreeWidgetItem *item_ptr = tree_widget.topLevelItem(path[0]);

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


QTreeWidgetItem* QtTreeEditor::findSelectedItem()
{
  QList<QTreeWidgetItem*> items = treeEditor().selectedItems();

  if (items.size()!=1) {
    return nullptr;
  }

  return items.front();
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


function<
  void(
    const string &operation_name,
    function<void(TreeOperationHandler &)> perform_function
  )
> QtTreeEditor::addMenuItemForOperationFunction(
    QMenu &menu,
    list<QtSlot> &item_slots
  )
{
  return [&](
    const string &operation_name,
    function<void(TreeOperationHandler &)> perform_function
  )
  {
    QAction &action = createAction(menu,operation_name);
    auto perform_operation_function =
      [perform_function,this](){ perform_function(operation_handler); };
    item_slots.emplace_back(perform_operation_function);
    item_slots.back().connectSignal(action,SIGNAL(triggered()));
  };
}


void QtTreeEditor::prepareMenu(const QPoint &pos)
{
  QtTreeEditor &tree_editor = treeEditor();
  QTreeWidgetItem *widget_item_ptr = tree_editor.itemAt(pos);
  TreePath path;

  if (widget_item_ptr) {
    path = itemPath(*widget_item_ptr);
  }

  QMenu menu;
  list<QtSlot> item_slots;
  tree().visitOperations(
    path,
    addMenuItemForOperationFunction(menu,item_slots)
  );
  menu.exec(tree_editor.mapToGlobal(pos));
}


void QtTreeEditor::prepareMenuSlot(const QPoint &pos)
{
  prepareMenu(pos);
}


void QtTreeEditor::selectItem(const TreePath &path)
{
  itemFromPath(path).setSelected(true);
}
