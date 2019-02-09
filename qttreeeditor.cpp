#include "qttreeeditor.hpp"

#include <iostream>
#include <type_traits>
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


namespace {
struct QtItemWidget : QWidget {
  QLabel *label_widget_ptr = nullptr;
};
}


static void setLabelWidgetText(QLabel &label_widget,const string &label)
{
  label_widget.setText(QString::fromStdString(label));
}


template <typename T>
T &
  QtTreeEditor::createItemWidget(
    QTreeWidgetItem &item,
    const LabelProperties &label_properties
  )
{
  const std::string &label = label_properties.text;
  bool label_is_editable = label_properties.is_editable;
  QtItemWidget *wrapper_widget_ptr = new QtItemWidget();
  // NOTE: setting the item widget before adding the contents makes
  // it not have the proper size.
  QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);

  if (label_is_editable) {
    assert(false); // not handled
    QtLineEdit &label_widget = createWidget<QtLineEdit>(layout);
    label_widget.setText(label);
    label_widget.text_changed_function =
      [&item,this](const string &new_text){
        itemLabelChanged(item, new_text);
      };
  }
  else {
    QLabel &label_widget = createWidget<QLabel>(layout);
    setLabelWidgetText(label_widget,label);
    wrapper_widget_ptr->label_widget_ptr = &label_widget;
  }

  T& widget = createWidget<T>(layout);
  setItemWidget(&item,/*column*/0,wrapper_widget_ptr);
  return widget;
}


void
  QtTreeEditor::itemLabelChanged(
    QTreeWidgetItem &item,
    const string &new_text
  )
{
  TreePath path = itemPath(item);
  TreeEditor::itemLabelChanged(path,new_text);
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
    const TreePath &new_item_path,
    const LabelProperties &label_properties
  )
{
  const string &label = label_properties.text;
  TreePath parent_path = parentPath(new_item_path);
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  QTreeWidgetItem &item = insertChildItem(parent_item,new_item_path.back());
  setItemText(item,label);
}


void
  QtTreeEditor::createNumericItem(
    const TreePath &new_item_path,
    const LabelProperties &label_properties,
    const NumericValue value,
    const NumericValue minimum_value,
    const NumericValue maximum_value
  )
{
  TreePath parent_path = parentPath(new_item_path);
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  int child_index = new_item_path.back();

  static_assert(std::is_same<NumericValue,int>::value,"");

  bool value_is_limited_on_both_ends =
    minimum_value != std::numeric_limits<NumericValue>::min() &&
    maximum_value != std::numeric_limits<NumericValue>::max();

  if (value_is_limited_on_both_ends) {
    createSliderItem(
      parent_item,
      child_index,
      label_properties,
      value,
      minimum_value,
      maximum_value
    );
  }
  else {
    createSpinBoxItem(
      parent_item,
      child_index,
      label_properties,
      value,
      minimum_value,
      maximum_value
    );
  }
}


void
  QtTreeEditor::createEnumerationItem(
    const TreePath &new_item_path,
    const LabelProperties &label_properties,
    const vector<string> &options,
    int value
  )
{
  TreePath parent_path = parentPath(new_item_path);
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  int index = new_item_path.back();
  createComboBoxItem(parent_item,index,label_properties,options,value);
}


void
  QtTreeEditor::createStringItem(
    const TreePath &new_item_path,
    const LabelProperties &label_properties,
    const string &value
  )
{
  TreePath parent_path = parentPath(new_item_path);
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  assert(new_item_path.back() == parent_item.childCount());
  createLineEditItem(parent_item,label_properties,value);
}


void QtTreeEditor::setItemText(QTreeWidgetItem &item,const std::string &label)
{
  item.setText(/*column*/0,QString::fromStdString(label));
}


void
  QtTreeEditor::setItemLabel(const TreePath &path,const std::string &new_label)
{
  QTreeWidgetItem &item = itemFromPath(path);
  QWidget *widget_ptr = itemWidget(&item,/*column*/0);
  QtItemWidget *item_widget_ptr = dynamic_cast<QtItemWidget*>(widget_ptr);
  assert(item_widget_ptr);
  assert(item_widget_ptr->label_widget_ptr);
  setLabelWidgetText(*item_widget_ptr->label_widget_ptr,new_label);
}


QTreeWidgetItem&
  QtTreeEditor::createComboBoxItem(
    QTreeWidgetItem &parent_item,
    int index,
    const LabelProperties &label_properties,
    const std::vector<std::string> &enumeration_names,
    int value
  )
{
  QTreeWidgetItem &item = ::insertChildItem(parent_item,index);
  QtComboBox &combo_box = createItemWidget<QtComboBox>(item,label_properties);
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
    const LabelProperties &label_properties,
    const std::string &value
  )
{
  QTreeWidgetItem &item = ::createChildItem(parent_item);
  QtLineEdit &line_edit =
    createItemWidget<QtLineEdit>(item,label_properties);
  line_edit.text_changed_function = [&](const string &new_text){
    handleLineEditItemValueChanged(&item,new_text);
  };
  line_edit.setText(value);
}


void
  QtTreeEditor::createSpinBoxItem(
    QTreeWidgetItem &parent_item,
    int child_index,
    const LabelProperties &label_properties,
    int value,
    int minimum_value,
    int maximum_value
  )
{
  QtTreeEditor &tree_widget = *this;
  QTreeWidgetItem &item = ::insertChildItem(parent_item,child_index);
  QtSpinBox &spin_box =
    tree_widget.createItemWidget<QtSpinBox>(item,label_properties);
  spin_box.setValue(value);
  spin_box.setMinimum(minimum_value);
  spin_box.setMaximum(maximum_value);
  spin_box.value_changed_function =
    [this,&item](int value){
      handleSpinBoxItemValueChanged(&item,value);
    };
}


void
  QtTreeEditor::createSliderItem(
    QTreeWidgetItem &parent_item,
    int child_index,
    const LabelProperties &label_properties,
    int value,
    int minimum_value,
    int maximum_value
  )
{
  QtTreeEditor &tree_widget = *this;
  QTreeWidgetItem &item = ::insertChildItem(parent_item,child_index);
  QSlider &slider =
    tree_widget.createItemWidget<QSlider>(item,label_properties);
  slider.setOrientation(Qt::Horizontal);
  slider.setValue(value);
  slider.setMinimum(minimum_value);
  slider.setMaximum(maximum_value);

  // Need to have a value changed function
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
  QTreeWidgetItem &item = itemFromPath(path);

  while (item.childCount()>0) {
    item.removeChild(item.child(item.childCount()-1));
  }
}


QTreeWidgetItem* QtTreeEditor::findSelectedItem()
{
  QList<QTreeWidgetItem*> items = selectedItems();

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
  QtTreeEditor &tree_editor = *this;
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
