#include "any.hpp"
#include "point2d.hpp"


struct Point2DObjectData : Object::Data {
  Point2D &point;

  Point2DObjectData(Point2D &point_arg) : point(point_arg) { }

  Optional<Any> maybeMember(const std::string &member_name) override
  {
    if (member_name=="x") {
      return {point.x};
    }

    if (member_name=="y") {
      return {point.y};
    }

    return {};
  }

  virtual Point2DObjectData *clone()
  {
    return new Point2DObjectData(*this);
  }

  std::vector<std::string> memberNames() const override
  {
    assert(false); // needs test
  }
};


extern Object makePoint2DObject(Point2D &point);
