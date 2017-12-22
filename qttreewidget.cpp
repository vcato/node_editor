#include "qttreewidget.hpp"

#include <iostream>

using std::cerr;


void
  QtTreeWidget::comboBoxItemCurrentIndexChangedSlot(
    QtComboBoxTreeWidgetItem *item_ptr,
    int index
  )
{
  emit comboBoxItemIndexChanged(item_ptr,index);
}

