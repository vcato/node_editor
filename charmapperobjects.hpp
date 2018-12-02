#include "any.hpp"
#include "scene.hpp"
#include "bodylink.hpp"


struct PosExprData {
  BodyLink body_link;
  Point2D position;
};


struct BodyLinkObjectData : Object::Data, BodyLink {
  virtual Data *clone() { return new auto(*this); }

  BodyLinkObjectData(const BodyLink &arg)
  : BodyLink(arg)
  {
  }

  std::string typeName() const override
  {
    return "BodyLink";
  }

  Any member(const std::string &member_name) const override;

  std::vector<std::string> memberNames() const override;
};


// This is the data for a dynamic PosExpr object.
struct PosExprObjectData : Object::Data, PosExprData {
  PosExprObjectData(BodyLink body_link_arg,const Point2D &position_arg)
  : PosExprData{body_link_arg,position_arg}
  {
  }

  std::string typeName() const override
  {
    return "PosExpr";
  }

  PosExprObjectData *clone() override
  {
    return new PosExprObjectData(*this);
  }

  Any member(const std::string &member_name) const override;

  std::vector<std::string> memberNames() const override
  {
    return {"body","position"};
  }
};


extern Optional<PosExprData> maybePosExpr(const Any &);
extern Class posExprClass();
