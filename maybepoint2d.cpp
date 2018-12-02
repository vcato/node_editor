#include "maybepoint2d.hpp"

#include "point2dobject.hpp"

using std::vector;


Optional<Point2D> maybePoint2D(const Any &arg)
{
  if (!arg.isVector()) {
    if (arg.isObject()) {
      const Object::Data *data_ptr = &arg.asObject().data();
      assert(data_ptr);
      auto point2d_data_ptr = dynamic_cast<const Point2DObjectData*>(data_ptr);

      if (!point2d_data_ptr) {
        assert(false);
      }

      return point2d_data_ptr->point;
    }

    return {};
  }

  const vector<Any> &arg_vector = arg.asVector();

  if (arg_vector.size()!=2) {
    return {};
  }

  const Any &any_x = arg_vector[0];
  const Any &any_y = arg_vector[1];

  if (!any_x.isFloat()) {
    return {};
  }

  if (!any_y.isFloat()) {
    return {};
  }

  return Point2D(any_x.asFloat(),any_y.asFloat());
}
