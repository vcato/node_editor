#include "qtscenetree.hpp"

#include <cassert>
#include <QHeaderView>
#include "qttreewidgetitem.hpp"


static void
  addBodiesTo(
    QTreeWidgetItem &parent_item,
    const SceneTree::Item::Children &item_children
  )
{
  for (auto &item : item_children) {
    QTreeWidgetItem &item1 = createChildItem(parent_item);
    item1.setText(/*column*/0,QString::fromStdString(item.label));
    addBodiesTo(item1,item.children);
  }
}


QtSceneTree::QtSceneTree()
{
  assert(header());
  header()->close();
}


void QtSceneTree::setItems(const Item &root)
{
  clear();
  assert(invisibleRootItem());
  addBodiesTo(*invisibleRootItem(),root.children);
}
