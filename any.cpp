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


Optional<Object>
  Class::maybeMakeObject(
    const NamedParameters &parameters,
    std::ostream &error_stream
  ) const
{
  assert(make_object_function);

  return make_object_function(parameters,error_stream);
}


bool Class::operator==(const Class &) const
{
  assert(false);
  return true;
}
