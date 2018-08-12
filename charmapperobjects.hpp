#include "any.hpp"
#include "scene.hpp"


struct PosExprObjectData : Object::Data {
  PosExprObjectData(Scene::Body *body_ptr_arg,const Point2D &position_arg)
  : body_ptr(body_ptr_arg),
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

  Scene::Body *body_ptr;
  Point2D position;
};

extern Class posExprClass();
