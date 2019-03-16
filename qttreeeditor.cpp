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
#include "qtslider.hpp"


using std::cerr;
using std::vector;
using std::ostream;
using std::string;
using std::function;
using std::list;


namespace {
struct QtItemWrapperWidget : QWidget {
  QLabel *label_widget_ptr = nullptr;
  QWidget *value_widget_ptr = nullptr;
};
}


static void setLabelWidgetText(QLabel &label_widget,const string &label)
{
  label_widget.setText(QString::fromStdString(label));
}


QLabel&
  QtTreeWidget::createItemLabelWidget(
    QTreeWidgetItem &,
    QHBoxLayout &layout,
    const LabelProperties &label_properties
  )
{
  const std::string &label = label_properties.text;
  bool label_is_editable = label_properties.is_editable;

  if (label_is_editable) {
    assert(false); // not handled
  }
  else {
    QLabel &label_widget = createWidget<QLabel>(layout);
    setLabelWidgetText(label_widget,label);
    return label_widget;
  }
}



template <typename T>
T &
  QtTreeWidget::createItemWidget(
    QTreeWidgetItem &item,
    const LabelProperties &label_properties
  )
{
  QtItemWrapperWidget *wrapper_widget_ptr = new QtItemWrapperWidget();
  // NOTE: setting the item widget before adding the contents makes
  // it not have the proper size.
  QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);

  wrapper_widget_ptr->label_widget_ptr =
    &createItemLabelWidget(item,layout,label_properties);

  T& widget = createWidget<T>(layout);

  wrapper_widget_ptr->value_widget_ptr = &widget;
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

  combobox_item_index_changed_function =
    [this](const TreePath &path,int index){
      treeComboBoxItemIndexChanged(path,index);
    };

  spin_box_item_value_changed_function =
    [this](const TreePath &path,int index){
      treeSpinBoxItemValueChanged(path,index);
    };

  slider_item_value_changed_function =
    [this](const TreePath &path,int value){
      treeSliderItemValueChanged(path,value);
    };

  line_edit_item_value_changed_function =
    [this](const TreePath &path,const string& value){
      treeLineEditItemValueChanged(path,value);
    };
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
  QtTreeWidget::createVoidItem(
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


static bool
  useSliderForRange(NumericValue minimum_value,NumericValue maximum_value)
{
  bool value_is_limited_on_both_ends =
    minimum_value != std::numeric_limits<NumericValue>::min() &&
    maximum_value != std::numeric_limits<NumericValue>::max();

  return value_is_limited_on_both_ends;
}


void
  QtTreeWidget::createNumericItem(
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

  if (useSliderForRange(minimum_value,maximum_value)) {
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


static QtItemWrapperWidget &
  itemWrapperWidget(QTreeWidget &tree,QTreeWidgetItem &item)
{
  QWidget *widget_ptr = tree.itemWidget(&item,/*column*/0);
  QtItemWrapperWidget *item_widget_ptr =
    dynamic_cast<QtItemWrapperWidget*>(widget_ptr);
  assert(item_widget_ptr);
  return *item_widget_ptr;
}


struct QtTreeWidget::Impl {
  static void
    setupSpinBox(
      QtTreeWidget &tree_widget,
      QtSpinBox &spin_box,
      QTreeWidgetItem &item
    )
  {
    spin_box.value_changed_function =
      [&tree_widget,&item](int value){
        tree_widget.handleSpinBoxItemValueChanged(&item,value);
      };
  }

  static void
    setupSlider(
      QtTreeWidget &tree_widget,
      QtSlider &slider,
      QTreeWidgetItem &item
    )
  {
    slider.value_changed_function =
      [&tree_widget,&item](int value){
        tree_widget.handleSliderItemValueChanged(&item,value);
      };
  }

  static QBoxLayout &boxLayout(QtItemWrapperWidget &wrapper_widget)
  {
    auto *box_layout_ptr =
      dynamic_cast<QBoxLayout *>(wrapper_widget.layout());
    assert(box_layout_ptr);
    QBoxLayout &box_layout = *box_layout_ptr;
    return box_layout;
  }

  static void
    destroyValueWidget(QtItemWrapperWidget &wrapper_widget)
  {
    QWidget *value_widget_ptr = wrapper_widget.value_widget_ptr;
    wrapper_widget.value_widget_ptr = 0;

    QBoxLayout &box_layout = boxLayout(wrapper_widget);
    box_layout.removeWidget(wrapper_widget.value_widget_ptr);

    delete value_widget_ptr;
  }

  static QtSlider &
    createSlider(
      QtTreeWidget &tree_widget,
      QtItemWrapperWidget &wrapper_widget,
      QTreeWidgetItem &item
    )
  {
    QBoxLayout &box_layout = boxLayout(wrapper_widget);
    auto& slider = createWidget<QtSlider>(box_layout);
    wrapper_widget.value_widget_ptr = &slider;
    setupSlider(tree_widget,slider,item);
    return slider;
  }

  static QtSpinBox &
    createSpinBox(
      QtTreeWidget &tree_widget,
      QtItemWrapperWidget &wrapper_widget,
      QTreeWidgetItem &item
    )
  {
    QBoxLayout &box_layout = boxLayout(wrapper_widget);
    auto& spin_box = createWidget<QtSpinBox>(box_layout);
    setupSpinBox(tree_widget,spin_box,item);
    wrapper_widget.value_widget_ptr = &spin_box;
    return spin_box;
  }
};


void
  QtTreeEditor::setItemNumericValue(
    const TreePath &path,
    NumericValue value,
    NumericValue minimum_value,
    NumericValue maximum_value
  )
{
  QTreeWidgetItem &item = itemFromPath(path);
  QtItemWrapperWidget &wrapper_widget = itemWrapperWidget(*this,item);
  QWidget *value_widget_ptr = wrapper_widget.value_widget_ptr;
  assert(value_widget_ptr);

  bool use_slider = useSliderForRange(minimum_value,maximum_value);
  auto *slider_ptr = dynamic_cast<QtSlider*>(value_widget_ptr);
  auto *spin_box_ptr = dynamic_cast<QtSpinBox*>(value_widget_ptr);

  if (use_slider && !slider_ptr) {
    assert(spin_box_ptr);
    spin_box_ptr = nullptr;
    Impl::destroyValueWidget(wrapper_widget);
    slider_ptr = &Impl::createSlider(*this,wrapper_widget,item);
  }

  if (!use_slider && !spin_box_ptr) {
    assert(slider_ptr);
    slider_ptr = nullptr;
    Impl::destroyValueWidget(wrapper_widget);
    spin_box_ptr = &Impl::createSpinBox(*this,wrapper_widget,item);
  }

  if (slider_ptr) {
    auto &slider = *slider_ptr;
    slider.setMinimum(minimum_value);
    slider.setMaximum(maximum_value);
    slider.setValue(value);
  }
  else {
    assert(spin_box_ptr);
    auto &spin_box = *spin_box_ptr;
    spin_box.setMinimum(minimum_value);
    spin_box.setMaximum(maximum_value);
    spin_box.setValue(value);
  }
}


void
  QtTreeWidget::createEnumerationItem(
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
  QtTreeWidget::createStringItem(
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


void QtTreeWidget::setItemText(QTreeWidgetItem &item,const std::string &label)
{
  item.setText(/*column*/0,QString::fromStdString(label));
}


void QtTreeEditor::setItemText(QTreeWidgetItem &item,const std::string &label)
{
  QtTreeWidget::setItemText(item,label);
}


void
  QtTreeEditor::setItemLabel(const TreePath &path,const std::string &new_label)
{
  QTreeWidgetItem &item = itemFromPath(path);

  QtItemWrapperWidget &item_widget = itemWrapperWidget(*this,item);
  assert(item_widget.label_widget_ptr);
  setLabelWidgetText(*item_widget.label_widget_ptr,new_label);
}


QTreeWidgetItem&
  QtTreeWidget::createComboBoxItem(
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
  QtTreeWidget::createLineEditItem(
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
  QtTreeWidget::createSpinBoxItem(
    QTreeWidgetItem &parent_item,
    int child_index,
    const LabelProperties &label_properties,
    int value,
    int minimum_value,
    int maximum_value
  )
{
  QtTreeWidget &tree_widget = *this;
  QTreeWidgetItem &item = ::insertChildItem(parent_item,child_index);
  QtSpinBox &spin_box =
    tree_widget.createItemWidget<QtSpinBox>(item,label_properties);
  spin_box.setValue(value);
  spin_box.setMinimum(minimum_value);
  spin_box.setMaximum(maximum_value);
  Impl::setupSpinBox(*this,spin_box,item);
}


void
  QtTreeWidget::createSliderItem(
    QTreeWidgetItem &parent_item,
    int child_index,
    const LabelProperties &label_properties,
    int value,
    int minimum_value,
    int maximum_value
  )
{
  QtTreeWidget &tree_widget = *this;
  QTreeWidgetItem &item = ::insertChildItem(parent_item,child_index);

  // Logic needs to be consistent when we recreate the widget if the value
  // changes.
  QtSlider &slider =
    tree_widget.createItemWidget<QtSlider>(item,label_properties);

  Impl::setupSlider(*this,slider,item);

  slider.setValue(value);
  slider.setMinimum(minimum_value);
  slider.setMaximum(maximum_value);

  // Need to have a value changed function
}



QTreeWidgetItem &QtTreeWidget::itemFromPath(const std::vector<int> &path) const
{
  int path_length = path.size();

  if (path_length==0) {
    assert(invisibleRootItem());
    return *invisibleRootItem();
  }

  assert(path_length>0);
  QTreeWidgetItem *item_ptr = topLevelItem(path[0]);

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


QTreeWidgetItem &QtTreeEditor::itemFromPath(const std::vector<int> &path) const
{
  return tree().itemFromPath(path);
}


int QtTreeEditor::itemChildCount(const TreePath &parent_path) const
{
  QTreeWidgetItem &parent_item = itemFromPath(parent_path);
  return parent_item.childCount();
}


void QtTreeEditor::removeItem(const TreePath &path)
{
  auto parent_path = parentPath(path);
  auto child_index = path.back();
  ::removeChildItem(itemFromPath(parent_path),child_index);
}


void QtTreeWidget::buildPath(vector<int> &path,QTreeWidgetItem &item)
{
  QTreeWidgetItem *parent_item_ptr = item.parent();

  if (!parent_item_ptr) {
    path.push_back(indexOfTopLevelItem(&item));
    return;
  }

  buildPath(path,*parent_item_ptr);
  path.push_back(parent_item_ptr->indexOfChild(&item));
}


std::vector<int> QtTreeWidget::itemPath(QTreeWidgetItem &item)
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


void QtTreeEditor::treeComboBoxItemIndexChanged(const TreePath &path,int index)
{
  setEnumerationIndex(path,index);
}


void QtTreeEditor::treeSpinBoxItemValueChanged(const TreePath &path,int value)
{
  numberItemValueChanged(path,value);
}


void
  QtTreeWidget::handleComboBoxItemIndexChanged(
    QTreeWidgetItem *item_ptr,
    int index
  )
{
  assert(item_ptr);

  TreePath path = itemPath(*item_ptr);
  combobox_item_index_changed_function(path,index);
}


void
  QtTreeWidget::handleSpinBoxItemValueChanged(
    QTreeWidgetItem *item_ptr,
    int value
  )
{
  assert(item_ptr);
  spin_box_item_value_changed_function(itemPath(*item_ptr),value);
}


void
  QtTreeEditor::treeSliderItemValueChanged(
    const TreePath &path,
    int value
  )
{
  numberItemValueChanged(path,value);
}


void
  QtTreeWidget::handleSliderItemValueChanged(
    QTreeWidgetItem *item_ptr,
    int value
  )
{
  assert(item_ptr);
  slider_item_value_changed_function(itemPath(*item_ptr),value);
}


void
  QtTreeEditor::treeLineEditItemValueChanged(
    const TreePath &path,
    const string &value
  )
{
  stringItemValueChanged(path,value);
}


void
  QtTreeWidget::handleLineEditItemValueChanged(
    QTreeWidgetItem *item_ptr,
    const string &value
  )
{
  assert(item_ptr);
  line_edit_item_value_changed_function(itemPath(*item_ptr),value);
}


void QtTreeEditor::itemSelectionChangedSlot()
{
}


DiagramEditorWindow& QtTreeEditor::createDiagramEditor()
{
  auto window_ptr = new QtDiagramEditorWindow;
  window_ptr->setAttribute( Qt::WA_DeleteOnClose );
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
