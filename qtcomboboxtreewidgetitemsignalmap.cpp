#include "qtcomboboxtreewidgetitemsignalmap.hpp"

#include <iostream>

using std::cerr;


void QtComboBoxTreeWidgetItemSignalMap::currentIndexChangedSlot(int index)
{
  emit currentIndexChanged(item_ptr,index);
}
