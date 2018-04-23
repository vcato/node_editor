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
#include "wrapperutil.hpp"
#include "qtcombobox.hpp"
#include "qtlineedit.hpp"
#include "qttreewidgetitem.hpp"
#include "qtdiagrameditorwindow.hpp"

using std::cerr;
using std::vector;
using std::ostream;
using std::string;
using std::function;
using std::list;


static vector<string> comboBoxItems(const EnumerationWrapper &wrapper)
{
  return wrapper.enumerationNames();
}


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
      parent_item,
      wrapper.label(),
      comboBoxItems(wrapper)
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


QtTreeEditor::QtTreeEditor()
{
  assert(header());
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
  QTreeWidgetItem &pass_item = ::createChildItem(parent_item);
  setItemText(pass_item,label);
  return pass_item;
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
  QTreeWidgetItem &item = ::createChildItem(parent_item);
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
  QTreeWidgetItem &item = ::createChildItem(parent_item);
  QtLineEdit &line_edit = createItemWidget<QtLineEdit>(item,label);
  line_edit.text_changed_function = [&](const string &new_text){
    handleLineEditItemValueChanged(&item,new_text);
  };
  line_edit.setText(value);
}


void
  QtTreeEditor::createSpinBoxItem(
    QTreeWidgetItem &parent_item,
    const std::string &label
  )
{
  QtTreeEditor &tree_widget = *this;
  QTreeWidgetItem &item = ::createChildItem(parent_item);
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


void QtTreeEditor::addMainTreeItem(const TreePath &new_item_path)
{
  TreePath parent_path = parentPath(new_item_path);
  TreeItemIndex child_index = new_item_path.back();

  QTreeWidgetItem &parent_item = itemFromPath(parent_path);

  if (child_index!=parent_item.childCount()) {
    assert(false); // not implemented
  }

  bool created = false;

  visitSubWrapper(
    world(),
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
}


void QtTreeEditor::removeTreeItem(const TreePath &path)
{
  auto parent_path = parentPath(path);
  auto child_index = path.back();
  ::removeChildItem(itemFromPath(parent_path),child_index);
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
  addChildTreeItems(parent_path);
  diagramEditor().setDiagramPtr(maybeSelectedDiagram());
  diagramEditor().redraw();
}


static vector<string>
  getComboBoxItems(const Wrapper &wrapper,const TreePath &path)
{
  vector<string> items;

  visitEnumerationSubWrapper(
    wrapper,
    path,
    [&](const EnumerationWrapper &enumeration_wrapper){
      items = comboBoxItems(enumeration_wrapper);
    }
  );

  return items;
}


void QtTreeEditor::changeEnumerationValues(const TreePath &path)
{
  vector<string> items = getComboBoxItems(world(),path);
  QTreeWidgetItem &item = itemFromPath(path);
  QWidget *widget_ptr = itemWidget(&item,/*column*/0);
  QtComboBoxTreeItemWidget *combobox_item_widget_ptr =
    dynamic_cast<QtComboBoxTreeItemWidget*>(widget_ptr);
  assert(combobox_item_widget_ptr);
  QtComboBox &combo_box = combobox_item_widget_ptr->comboBox();
  combo_box.setItems(items);
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
  setEnumerationIndex(path,index);
}


void
  QtTreeEditor::handleSpinBoxItemValueChanged(
    QTreeWidgetItem *item_ptr,
    int value
  )
{
  assert(item_ptr);

  TreePath path = itemPath(*item_ptr);

  visitNumericSubWrapper(
    world(),
    path,
    [&](const NumericWrapper &numeric_wrapper){
      numeric_wrapper.setValue(value);
    }
  );
}


void
  QtTreeEditor::handleLineEditItemValueChanged(
    QTreeWidgetItem *item_ptr,
    const string &value
  )
{
  assert(item_ptr);
  stringItemValueChanged(itemPath(*item_ptr),value);
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
    diagram_ptr = diagramPtr(world(),itemPath(*selected_item_ptr));
  }

  return diagram_ptr;
}


void QtTreeEditor::itemSelectionChangedSlot()
{
  diagramEditor().setDiagramPtr(maybeSelectedDiagram());
}


QtDiagramEditorWindow& QtTreeEditor::createDiagramEditor()
{
  auto window_ptr = new QtDiagramEditorWindow;
  window_ptr->show();
  return *window_ptr;
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

  std::vector<std::string> operation_names = operationNames(path);

  int n_operations = operation_names.size();

  for (int i=0; i!=n_operations; ++i) {
    createAction(menu,operation_names[i],[&,i]{ executeOperation(path,i); });
  }

  if (diagramPtr(world(),path)) {
    createAction(menu,"Edit Diagram...",[&]{ openDiagramEditor(path); });
  }

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


void QtTreeEditor::setDiagramEditorPtr(QtDiagramEditor *arg)
{
  diagram_editor_ptr = arg;
}
