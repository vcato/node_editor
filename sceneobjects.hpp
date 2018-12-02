#include "bodylink.hpp"
#include "any.hpp"


struct BodyObjectData : Object::Data {
  BodyObjectData(BodyLink body_link_arg)
  : body_link(body_link_arg)
  {
  }

  Data *clone() override { return new BodyObjectData(*this); }
  std::string typeName() const override { return "Body"; }
  Any member(const std::string &member_name) const override;
  std::vector<std::string> memberNames() const override;

  BodyLink body_link;
};


extern Object bodyObject(BodyLink body_link);
