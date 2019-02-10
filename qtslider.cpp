#include "qtslider.hpp"

#include <iostream>

using std::cerr;


QtSlider::QtSlider()
{
  setOrientation(Qt::Horizontal);
  connect(this,SIGNAL(valueChanged(int)),SLOT(valueChangedSlot(int)));
}


void QtSlider::valueChangedSlot(int value)
{
  if (value_changed_function) {
    value_changed_function(value);
  }
}
