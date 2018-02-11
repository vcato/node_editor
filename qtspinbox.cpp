#include "qtspinbox.hpp"


QtSpinBox::QtSpinBox()
{
  connect(
    this,
    SIGNAL(valueChanged(int)),
    SLOT(valueChangedSlot(int))
  );
}


void QtSpinBox::valueChangedSlot(int value)
{
  value_changed_function(value);
}
