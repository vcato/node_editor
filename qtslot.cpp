#include "qtslot.hpp"

#include <cassert>


using std::function;


QtSlot::QtSlot(function<void()> function_arg)
: slot_function(function_arg)
{
}


void QtSlot::slot()
{
  slot_function();
}
