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


void QtComboBox::addItems(const std::vector<std::string> &names)
{
  ignore_signals = true;

  for (auto &name : names) {
    addItem(QString::fromStdString(name));
  }

  ignore_signals = false;
}
