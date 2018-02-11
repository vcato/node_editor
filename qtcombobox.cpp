#include "qtcombobox.hpp"

#include <iostream>

using std::cerr;


QtComboBox::QtComboBox()
: ignore_signals(false)
{
  connect(
    this,
    SIGNAL(currentIndexChanged(int)),
    SLOT(currentIndexChangedSlot(int))
  );
}


void QtComboBox::currentIndexChangedSlot(int index)
{
  if (ignore_signals) return;

  cerr << "QtComboBox::currentIndexChangedSlot()\n";
  current_index_changed_function(index);
}
