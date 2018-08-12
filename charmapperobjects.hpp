#include "any.hpp"
#include "scene.hpp"
#include "bodylink.hpp"


struct PosExprObjectData : Object::Data {
  PosExprObjectData(BodyLink body_link_arg,const Point2D &position_arg)
  : body_link(body_link_arg),
    position(position_arg)
  {
  }

  PosExprObjectData *clone() override
  {
    return new PosExprObjectData(*this);
  }

  Optional<Any> member(const std::string &/*member_name*/) override
  {
    assert(false);
  }

  BodyLink body_link;
  Point2D position;
};

extern Class posExprClass();
