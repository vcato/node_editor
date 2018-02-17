#include "qttreeeditor.hpp"

#include <iostream>
#include <QMenu>
#include <QHeaderView>
#include <QLineEdit>
#include "diagramio.hpp"
#include "qtmenu.hpp"
#include "qtslot.hpp"
#include "qtspinbox.hpp"
#include "streamvector.hpp"
#include "qtcombobox.hpp"


using std::cerr;
using std::vector;
using std::ostream;
using std::string;
using std::function;
using std::list;


struct QtTreeEditor::CreateChildItemVisitor : Wrapper::Visitor {
  QtTreeEditor &tree_editor;
  QTreeWidgetItem &parent_item;
  bool &created;

  CreateChildItemVisitor(
    QtTreeEditor &tree_editor_arg,
    QTreeWidgetItem &parent_item_arg,
    bool &created_arg
  )
  : tree_editor(tree_editor_arg),
    parent_item(parent_item_arg),
    created(created_arg)
  {
  }

  void operator()(const VoidWrapper &wrapper) const override
  {
    tree_editor.createChildItem(parent_item,wrapper.label());
    created = true;
  }

  void operator()(const NumericWrapper &wrapper) const override
  {
    tree_editor.createSpinBoxItem(parent_item,wrapper.label());
    created = true;
  }

  void operator()(const EnumerationWrapper &wrapper) const override
  {
    tree_editor.createComboBoxItem(
      parent_item,wrapper.label(),wrapper.enumerationNames()
    );
    created = true;
  }

  void operator()(const StringWrapper &wrapper) const override
  {
    tree_editor.createLineEditItem(
      parent_item,wrapper.label(),wrapper.value()
    );
    created = true;
  }
};


struct QtTreeEditor::OperationHandler : TreeOperationHandler {
  QtTreeEditor &tree_editor;

  OperationHandler(QtTreeEditor &tree_editor_arg)
  : tree_editor(tree_editor_arg)
  {
  }

  virtual void addItem(const TreePath &path)
  {
    tree_editor.addTreeItem(path);
  }

  virtual void replaceTreeItems(const TreePath &path)
  {
    tree_editor.replaceTreeItems(path);
  }

  virtual void changeEnumerationValues(const TreePath &path) const
  {
    tree_editor.changeEnumerationValues(path);
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


namespace {
struct QtComboBoxTreeItemWidget : QWidget {
  QtComboBox *combo_box_ptr;

  QtComboBox &comboBox()
  {
    assert(combo_box_ptr);
    return *combo_box_ptr;
  }

  QtComboBoxTreeItemWidget(const string &label)
  : combo_box_ptr(0)
  {
    QHBoxLayout &layout = createLayout<QHBoxLayout>(*this);
    QLabel &label_widget = createWidget<QLabel>(layout);
    label_widget.setText(QString::fromStdString(label));
    QtComboBox& combo_box = createWidget<QtComboBox>(layout);
    combo_box_ptr = &combo_box;
  }
};
}


QTreeWidgetItem&
  QtTreeEditor::createComboBoxItem(
    QTreeWidgetItem &parent_item,
    const std::string &label,
    const std::vector<std::string> &enumeration_names
  )
{
  QTreeWidgetItem &item = createChildItem(parent_item);
  QtComboBoxTreeItemWidget *widget_ptr =
    new QtComboBoxTreeItemWidget(label);
  QtComboBox &combo_box = widget_ptr->comboBox();
  setItemWidget(&item,/*column*/0,widget_ptr);
  combo_box.current_index_changed_function =
    [this,&item](int index){
      handleComboBoxItemIndexChanged(&item,index);
    };
  combo_box.setItems(enumeration_names);
  return item;
}


void
  QtTreeEditor::createLineEditItem(
    QTreeWidgetItem &parent_item,
    const std::string &label,
    const std::string &value
  )
{
  QTreeWidgetItem &item = createChildItem(parent_item);
  QLineEdit &line_edit = createItemWidget<QLineEdit>(item,label);
  line_edit.setText(QString::fromStdString(value));
}


void
  QtTreeEditor::createSpinBoxItem(
    QTreeWidgetItem &parent_item,
    const std::string &label
  )
{
  QtTreeEditor &tree_widget = *this;
  QTreeWidgetItem &item = createChildItem(parent_item);
  QtSpinBox &spin_box = tree_widget.createItemWidget<QtSpinBox>(item,label);
  spin_box.value_changed_function =
    [this,&item](int value){
      handleSpinBoxItemValueChanged(&item,value);
    };
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


Wrapper &QtTreeEditor::world()
{
  assert(world_ptr);
  return *world_ptr;
}


void QtTreeEditor::addTreeItem(const TreePath &new_item_path)
{
  TreePath parent_path = parentPath(new_item_path);
  TreeItemIndex child_index = new_item_path.back();

  QTreeWidgetItem &parent_item = itemFromPath(parent_path);

  if (child_index!=parent_item.childCount()) {
    assert(false); // not implemented
  }

  bool created = false;

  world().visitWrapper(
    new_item_path,
    [&](const Wrapper &w){
      CreateChildItemVisitor
	create_child_item_visitor(*this,parent_item,created);

      w.accept(create_child_item_visitor);
    }
  );

  if (!created) {
    cerr << "No item created for parent " << parent_path << "\n";
    assert(created);
  }

  addTreeItems(new_item_path);
}


void QtTreeEditor::addTreeItems(const TreePath &parent_path)
{
  int n_children = world().nChildren(parent_path);
  for (int i=0; i!=n_children; ++i) {
    addTreeItem(join(parent_path,i));
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


namespace {
struct EnumerationVisitor : Wrapper::Visitor {
  using Function = std::function<void(const EnumerationWrapper &)>;
  Function function;

  EnumerationVisitor(const Function &function_arg)
  : function(function_arg)
  {
  }

  virtual void operator()(const VoidWrapper &) const
  {
    assert(false);
  }

  virtual void operator()(const NumericWrapper &) const
  {
    assert(false);
  }

  virtual void operator()(const EnumerationWrapper &wrapper) const
  {
    function(wrapper);
  }

  virtual void operator()(const StringWrapper &) const
  {
    assert(false);
  }
};
}


static void
  visitEnumeration(
    const Wrapper &wrapper,
    const TreePath &path,
    std::function<void(const EnumerationWrapper &)> f
  )
{
  wrapper.visitWrapper(
    path,
    [&](const Wrapper &sub_wrapper){
      sub_wrapper.accept(EnumerationVisitor(f));
    }
  );
}


#if 1
static vector<string>
  getEnumerationNames(const Wrapper &wrapper,const TreePath &path)
{
  vector<string> enumeration_names;

  visitEnumeration(
    wrapper,
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      enumeration_names = enumeration_wrapper.enumerationNames();
    }
  );

  return enumeration_names;
}
#endif


void QtTreeEditor::changeEnumerationValues(const TreePath &path)
{
  vector<string> enumeration_names = getEnumerationNames(world(),path);
  QTreeWidgetItem &item = itemFromPath(path);
  QWidget *widget_ptr = itemWidget(&item,/*column*/0);
  QtComboBoxTreeItemWidget *combobox_item_widget_ptr =
    dynamic_cast<QtComboBoxTreeItemWidget*>(widget_ptr);
  assert(combobox_item_widget_ptr);
  QtComboBox &combo_box = combobox_item_widget_ptr->comboBox();
  combo_box.setItems(enumeration_names);
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
    QTreeWidgetItem *item_ptr,
    int index
  )
{
  assert(item_ptr);

  TreePath path = itemPath(*item_ptr);
  OperationHandler operation_handler(*this);


  visitEnumeration(
    world(),
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      enumeration_wrapper.comboBoxItemIndexChanged(
        path,index,operation_handler
      );
    }
  );

}


void
  QtTreeEditor::handleSpinBoxItemValueChanged(
    QTreeWidgetItem *item_ptr,
    int value
  )
{
  assert(item_ptr);

  TreePath path = itemPath(*item_ptr);

  struct NumericVisitor : Wrapper::Visitor {
    using Function = std::function<void(const NumericWrapper &)>;
    Function function;

    NumericVisitor(const Function &function_arg)
    : function(function_arg)
    {
    }

    virtual void operator()(const VoidWrapper &) const
    {
      assert(false);
    }

    virtual void operator()(const NumericWrapper &wrapper) const
    {
      function(wrapper);
    }

    virtual void operator()(const EnumerationWrapper &) const
    {
      assert(false);
    }

    virtual void operator()(const StringWrapper &) const
    {
      assert(false);
    }
  };

  world().visitWrapper(
    path,
    [&](const Wrapper &wrapper){
      wrapper.accept(
	NumericVisitor(
	  [&](const NumericWrapper &numeric_wrapper){
	    numeric_wrapper.setValue(value);
	  }
	)
      );
    }
  );
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
    diagram_ptr = world().diagramPtr(itemPath(*selected_item_ptr));
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
  TreePath path;

  if (widget_item_ptr) {
    path = itemPath(*widget_item_ptr);
  }

  QMenu menu;
  list<QtSlot> item_slots;
  world().visitWrapper(
    path,
    [&](const Wrapper &wrapper){
      std::vector<std::string> operation_names = wrapper.operationNames();
      int n_operations = operation_names.size();

      for (int i=0; i!=n_operations; ++i) {
        QAction &action = createAction(menu,operation_names[i]);
        auto perform_operation_function =
          [this,i,path](){
            world().visitWrapper(
              path,
              [&](const Wrapper &wrapper){
                OperationHandler operation_handler(*this);
                wrapper.executeOperation(i,path,operation_handler);
              }
            );
          };
        item_slots.emplace_back(perform_operation_function);
        item_slots.back().connectSignal(action,SIGNAL(triggered()));
      }
    }
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
