#include "qttreeeditor.hpp"

#include <iostream>
#include <sstream>
#include <QMenu>
#include <QHeaderView>
#include "diagramio.hpp"
#include "qtmenu.hpp"
#include "qtslot.hpp"
#include "defaultdiagrams.hpp"
#include "streamvector.hpp"


using std::cerr;
using std::vector;
using std::ostream;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::function;
using std::list;


#if 1
struct QtTreeEditor::CreateChildItemVisitor : Tree::ItemVisitor {
  QtTreeEditor &tree_editor;
  const TreePath &parent_path;
  bool &created;

  CreateChildItemVisitor(
    QtTreeEditor &tree_editor_arg,
    const TreePath &parent_path_arg,
    bool &created_arg
  )
  : tree_editor(tree_editor_arg),
    parent_path(parent_path_arg),
    created(created_arg)
  {
  }

  void voidItem(const std::string &label) const override
  {
    tree_editor.createVoidChildItem(parent_path,label);
    created = true;
  }

  void numericItem(const std::string &label) const override
  {
    tree_editor.createNumericChildItem(parent_path,label);
    created = true;
  }

  void
    enumeratedItem(
      const std::string &label,
      const std::vector<std::string> &enumeration_names
    ) const override
  {
    tree_editor.createEnumeratedChildItem(parent_path,label,enumeration_names);
    created = true;
  }
};
#else
struct QtTreeEditor::CreateItemVisitor : Tree::ItemVisitor {
  QtTreeEditor &tree_editor;
  const TreePath &path;
  bool &created;

  CreateItemVisitor(
    QtTreeEditor &tree_editor_arg,
    const TreePath &path_arg,
    bool &created_arg
  )
  : tree_editor(tree_editor_arg),
    path(path_arg),
    created(created_arg)
  {
  }

  void voidItem(const std::string &label) const override
  {
    tree_editor.createVoidItem(path,label);
    created = true;
  }

  void numericItem(const std::string &label) const override
  {
    tree_editor.createNumericItem(path,label);
    created = true;
  }

  void
    enumeratedItem(
      const std::string &label,
      const std::vector<std::string> &enumeration_names
    ) const override
  {
    tree_editor.createEnumeratedItem(path,label,enumeration_names);
    created = true;
  }
};
#endif


struct QtTreeEditor::OperationHandler : TreeOperationHandler {
  QtTreeEditor &tree_editor;

  OperationHandler(QtTreeEditor &tree_editor_arg)
  : tree_editor(tree_editor_arg)
  {
  }

  virtual void addChildItem(const TreePath &path)
  {
    tree_editor.addTreeChildItem(path);
  }

  virtual void replaceTreeItems(const TreePath &path)
  {
    tree_editor.replaceTreeItems(path);
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
  QtTreeEditor::createChildItem(
    QTreeWidgetItem &parent_item,
    const std::string &label
  )
{
  QTreeWidgetItem &pass_item = createChildItem(parent_item);
  setItemText(pass_item,label);
  return pass_item;
}


QTreeWidgetItem& QtTreeEditor::createChildItem(QTreeWidgetItem &parent_item)
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
  QTreeWidgetItem &item = createChildItem(parent_item);
  tree_widget.createItemWidget<QSpinBox>(item,label);
}


void
  QtTreeEditor::createVoidChildItem(
    const TreePath &parent_path,
    const string &label
  )
{
  createChildItem(itemFromPath(parent_path),label);
}


struct QtTreeEditor::VoidItemSpec : QtTreeEditor::ItemSpec {
  const std::string &label;

  VoidItemSpec(const std::string &label_arg)
  : label(label_arg)
  {
  }

  void
    createChildItem(
      QtTreeEditor &editor,
      const TreePath &parent_path
    ) const override
  {
    editor.createVoidChildItem(parent_path,label);
  }
};


void QtTreeEditor::createItem(const TreePath &path,const ItemSpec &spec)
{
  TreePath parent_path = parentPath(path);
  int child_index = path.back();

  if (child_index!=itemFromPath(parent_path).childCount()) {
    assert(false); // not implemented
  }

  spec.createChildItem(*this,parent_path);
}

void
  QtTreeEditor::createVoidItem(
    const TreePath &path,
    const string &label
  )
{
  createItem(path,VoidItemSpec{label});
}


void
  QtTreeEditor::createNumericChildItem(
    const TreePath &parent_path,
    const string &label
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  createSpinBoxItem(parent_item,label);
}


void
  QtTreeEditor::createEnumeratedChildItem(
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

  if (!item_ptr) {
    cerr << "No item for path " << path << "\n";
  }

  assert(item_ptr);

  return *item_ptr;
}


Tree &QtTreeEditor::tree()
{
  assert(tree_ptr);
  return *tree_ptr;
}


void QtTreeEditor::addTreeChildItem(const TreePath &parent_path)
{
  Tree &tree = this->tree();
  Tree::Index child_index = tree.nChildItems(parent_path);
  TreePath new_item_path = join(parent_path,child_index);
  tree.createItem(new_item_path);

  cerr << "QtTreeEditor::addTreeITem: new_item_path=" <<
    new_item_path << "\n";

  bool created = false;
#if 1
  CreateChildItemVisitor create_child_item_visitor(*this,parent_path,created);
  tree.visitType(new_item_path,create_child_item_visitor);
#else
  CreateItemVisitor create_item_visitor(*this,new_item_path,created);
  tree.visitType(new_item_path,create_item_visitor);
#endif
  if (!created) {
    cerr << "No item created for parent " << parent_path << "\n";
    assert(created);
  }

  addTreeItems(new_item_path);
}


void QtTreeEditor::addTreeItems(const TreePath &parent_path)
{
  int n_children = tree().findNChildren(parent_path);
  for (int i=0; i!=n_children; ++i) {
    addTreeChildItem(parent_path);
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


void QtTreeEditor::replaceTreeItems(const TreePath &parent_path)
{
  removeChildItems(parent_path);
  addTreeItems(parent_path);
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
