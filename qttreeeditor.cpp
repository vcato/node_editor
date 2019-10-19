#include "qttreeeditor.hpp"

#include <QMenu>
#include "qtmenu.hpp"
#include "qtdiagrameditorwindow.hpp"


using std::vector;
using std::string;


QtTreeEditor::QtTreeEditor()
{
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


int QtTreeEditor::itemChildCount(const TreePath &parent_path) const
{
  return QtTreeWidget::itemChildCount(parent_path);
}


void QtTreeEditor::removeChildItems(const TreePath &path)
{
  QtTreeWidget::removeChildItems(path);
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
  QtTreeEditor::treeSliderItemValueChanged(
    const TreePath &path,
    int value
  )
{
  numberItemValueChanged(path,value);
}


void
  QtTreeEditor::treeLineEditItemValueChanged(
    const TreePath &path,
    const string &value
  )
{
  stringItemValueChanged(path,value);
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


void QtTreeEditor::setItemExpanded(const TreePath &path,bool new_expanded_state)
{
  QtTreeWidget::setItemExpanded(path,new_expanded_state);
}
