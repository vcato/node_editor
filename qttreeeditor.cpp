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


struct QtTreeEditor::CreateItemVisitor : Tree::ItemVisitor {
  QtTreeEditor &tree_editor;
  const TreePath &parent_path;

  CreateItemVisitor(
    QtTreeEditor &tree_editor_arg,
    const TreePath &parent_path_arg
  )
  : tree_editor(tree_editor_arg),
    parent_path(parent_path_arg)
  {
  }

  void voidItem(const std::string &label) const override
  {
    tree_editor.createVoidItem(parent_path,label);
  }

  void numericItem(const std::string &label) const override
  {
    tree_editor.createNumericItem(parent_path,label);
  }

  void
    enumeratedItem(
      const std::string &label,
      const std::vector<std::string> &enumeration_names
    ) const override
  {
    tree_editor.createEnumeratedItem(parent_path,label,enumeration_names);
  }
};


struct QtTreeEditor::OperationHandler : TreeOperationHandler {
  QtTreeEditor &tree_editor;

  OperationHandler(QtTreeEditor &tree_editor_arg)
  : tree_editor(tree_editor_arg)
  {
  }

  virtual void addItem(const TreePath &path,const TreeItem &item)
  {
    tree_editor.addTreeItem(path,item);
  }

  virtual void replaceTreeItems(const TreePath &path,const TreeItem &items)
  {
    tree_editor.replaceTreeItems(path,items);
  }
};


QtTreeEditor::QtTreeEditor()
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
  QtTreeEditor::createVoidItem(const TreePath &parent_path,const string &label)
{  
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  createItem(parent_item,label);
}


void
  QtTreeEditor::createNumericItem(
    const TreePath &parent_path,
    const string &label
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  createSpinBoxItem(parent_item,label);
}


void
  QtTreeEditor::createEnumeratedItem(
    const TreePath &parent_path,
    const string &label,
    const vector<string> &enumeration_names
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);

  QtComboBoxTreeWidgetItem &global_position_item =
    createComboBoxItem(parent_item,label);
  {
    QComboBox &combo_box = global_position_item.comboBox();
    ignore_combo_box_signals = true;
    for (auto &name : enumeration_names) {
      combo_box.addItem(QString::fromStdString(name));
    }
    ignore_combo_box_signals = false;
  }
}


void
  QtTreeEditor::addTreeItem(
    const TreePath &parent_path,
    const TreeItem &item
  )
{
  Tree &tree = this->tree();
  TreePath new_item_path = tree.createItem(parent_path,item);

  CreateItemVisitor create_item_visitor(*this,parent_path);
  tree.visitItem(item,create_item_visitor);

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
  diagramEditor().setDiagramPtr(maybeSelectedDiagram());
  diagramEditor().redraw();
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
  OperationHandler operation_handler(*this);

  tree().comboBoxItemIndexChanged(path,index,operation_handler);
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
    diagram_ptr = tree().itemDiagramPtr(itemPath(*selected_item_ptr));
  }

  return diagram_ptr;
}


void QtTreeEditor::itemSelectionChangedSlot()
{
  diagramEditor().setDiagramPtr(maybeSelectedDiagram());
}


static Tree::OperationVisitor
  addMenuItemForOperationFunction(
    QMenu &menu,
    list<QtSlot> &item_slots,
    TreeOperationHandler &operation_handler
  )
{
  return
    [&](
      const string &operation_name,
      function<void(TreeOperationHandler &)> perform_function
    )
    {
      QAction &action = createAction(menu,operation_name);
      auto perform_operation_function =
        [&operation_handler,perform_function](){
          perform_function(operation_handler);
        };
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
  OperationHandler operation_handler(*this);
  tree().visitOperations(
    path,
    addMenuItemForOperationFunction(menu,item_slots,operation_handler)
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
