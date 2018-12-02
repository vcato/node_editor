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

  Optional<Any> maybeMember(const std::string &member_name) override
  {
    if (member_name=="scene_name") {
      if (hasValue()) {
        assert(false);
      }
      else {
        return Any();
      }
    }

    if (member_name=="body_name") {
      if (hasValue()) {
        assert(false);
      }
      else {
        return Any();
      }
    }

    std::cerr << "member_name: " << member_name << "\n";
    assert(false); // needs test
  }

  virtual std::vector<std::string> memberNames() const
  {
    return {"scene_name","body_name"};
  }
};


// This is the data for a dynamic PosExpr object.
struct PosExprObjectData : Object::Data, PosExprData {
  PosExprObjectData(BodyLink body_link_arg,const Point2D &position_arg)
  : PosExprData{body_link_arg,position_arg}
  {
  }

  std::string typeName() const override
  {
    assert(false);
  }

  PosExprObjectData *clone() override
  {
    return new PosExprObjectData(*this);
  }

  Optional<Any> maybeMember(const std::string &/*member_name*/) override
  {
    assert(false);
  }

  std::vector<std::string> memberNames() const override
  {
    assert(false); // needs test
  }
};


extern Optional<PosExprData> maybePosExpr(const Any &);
extern Class posExprClass();
