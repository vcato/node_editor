#include "qttreeeditor.hpp"

#include <iostream>
#include <QMenu>
#include <QHeaderView>
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>
#include "qtmenu.hpp"
#include "qtslot.hpp"
#include "qtspinbox.hpp"
#include "streamvector.hpp"
#include "qtcombobox.hpp"
#include "qtlineedit.hpp"
#include "qttreewidgetitem.hpp"
#include "qtdiagrameditorwindow.hpp"
#include "qtwidget.hpp"
#include "qtlayout.hpp"


using std::cerr;
using std::vector;
using std::ostream;
using std::string;
using std::function;
using std::list;


template <typename T>
T &
  QtTreeEditor::createItemWidget(
    QTreeWidgetItem &item,
    const std::string &label
  )
{
  QWidget *wrapper_widget_ptr = new QWidget();
  // NOTE: setting the item widget before adding the contents makes
  // it not have the proper size.
  QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);
  QLabel &label_widget = createWidget<QLabel>(layout);
  label_widget.setText(QString::fromStdString(label));
  T& widget = createWidget<T>(layout);
  setItemWidget(&item,/*column*/0,wrapper_widget_ptr);
  return widget;
}


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


void
  QtTreeEditor::createVoidItem(
    const TreePath &parent_path,
    const string &label
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  createChildItem(parent_item,label);
}


void
  QtTreeEditor::createNumericItem(
    const TreePath &parent_path,
    const string &label,
    const NumericValue value
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  createSpinBoxItem(parent_item,label,value);
}


void
  QtTreeEditor::createEnumerationItem(
    const TreePath &parent_path,
    const string &label,
    const vector<string> &options,
    int value
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  createComboBoxItem(parent_item,label,options,value);
}


void
  QtTreeEditor::createStringItem(
    const TreePath &parent_path,
    const string &label,
    const string &value
  )
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  createLineEditItem(parent_item,label,value);
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
    const std::vector<std::string> &enumeration_names,
    int value
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
  combo_box.setIndex(value);
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
    const std::string &label,
    int value
  )
{
  QtTreeEditor &tree_widget = *this;
  QTreeWidgetItem &item = ::createChildItem(parent_item);
  QtSpinBox &spin_box = tree_widget.createItemWidget<QtSpinBox>(item,label);
  spin_box.setValue(value);
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


int QtTreeEditor::itemChildCount(const TreePath &parent_path) const
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  return parent_item.childCount();
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


void
  QtTreeEditor::setEnumerationValues(
    const TreePath &path,
    const vector<string> &items
  )
{
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
  numberItemValueChanged(itemPath(*item_ptr),value);
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


void QtTreeEditor::itemSelectionChangedSlot()
{
}


DiagramEditorWindow& QtTreeEditor::createDiagramEditor()
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

  vector<MenuItem> menu_items = contextMenuItems(path);

  QMenu menu;

  for (auto &item : menu_items) {
    createAction(menu,item.label,item.callback);
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


void QtTreeEditor::setItemExpanded(const TreePath &path,bool new_expanded_state)
{
  itemFromPath(path).setExpanded(new_expanded_state);
}
