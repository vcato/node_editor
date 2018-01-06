#include "qttreeeditor.hpp"

#include <iostream>

using std::cerr;


void
  QtTreeEditor::comboBoxItemCurrentIndexChangedSlot(
    QtComboBoxTreeWidgetItem *item_ptr,
    int index
  )
{
  emit comboBoxItemIndexChanged(item_ptr,index);
}

