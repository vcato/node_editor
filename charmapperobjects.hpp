#include "any.hpp"
#include "scene.hpp"
#include "bodylink.hpp"


struct PosExprData {
  BodyLink body_link;
  Point2D position;
};


struct PosExprObjectData : Object::Data, PosExprData {
  PosExprObjectData(BodyLink body_link_arg,const Point2D &position_arg)
  : PosExprData{body_link_arg,position_arg}
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

  void printOn(std::ostream &) const override
  {
    assert(false);
  }
};

extern Optional<PosExprData> maybePosExpr(const Any &);
extern Class posExprClass();
