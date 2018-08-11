#include "maybepoint2d.hpp"

using std::vector;


Optional<Point2D> maybePoint2D(const Any &arg)
{
  if (!arg.isVector()) {
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
