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

  Any member(const std::string &member_name) const override
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
    return "PosExpr";
  }

  PosExprObjectData *clone() override
  {
    return new PosExprObjectData(*this);
  }

  Any member(const std::string &member_name) const override
  {
    if (member_name=="body") {
      Object body_link_object(std::make_unique<BodyLinkObjectData>(body_link));
      body_link_object.data();
      return {Any(std::move(body_link_object))};
    }

    if (member_name=="position") {
      assert(false);
    }
    else {
      assert(false);
    }
  }

  std::vector<std::string> memberNames() const override
  {
    return {"body","position"};
  }
};


extern Optional<PosExprData> maybePosExpr(const Any &);
extern Class posExprClass();
