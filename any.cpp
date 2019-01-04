#include "any.hpp"


Object &Object::operator=(const Object &arg)
{
  if (&arg==this) {
    assert(false);
  }

  assert(arg.data_ptr);

  data_ptr.reset(arg.data_ptr->clone());
  return *this;
}
