#include "any.hpp"
#include "point2d.hpp"


struct Point2DObjectData : Object::Data {
  Point2D &point;

  Point2DObjectData(Point2D &point_arg) : point(point_arg) { }

  std::string typeName() const override
  {
    assert(false);
  }

  Any member(const std::string &member_name) const override
  {
    if (member_name=="x") {
      return point.x;
    }

    if (member_name=="y") {
      return point.y;
    }

    assert(false);
  }

  virtual Point2DObjectData *clone()
  {
    return new Point2DObjectData(*this);
  }

  std::vector<std::string> memberNames() const override
  {
    return {"x","y"};
  }
};


extern Object makePoint2DObject(Point2D &point);
