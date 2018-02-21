#include "qttreewidgetitem.hpp"


QTreeWidgetItem& createChildItem(QTreeWidgetItem &parent_item)
{
  QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
  parent_item.addChild(item_ptr);
  QTreeWidgetItem &item = *item_ptr;
  item.setExpanded(true);
  return item;
}
