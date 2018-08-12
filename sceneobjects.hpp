#include "bodylink.hpp"
#include "any.hpp"


struct BodyObjectData : Object::Data {
  BodyObjectData(BodyLink body_link_arg)
  : body_link(body_link_arg)
  {
  }

  Data *clone() override { return new BodyObjectData(*this); }

  Optional<Any> member(const std::string &member_name) override;

  ~BodyObjectData() override {}

  BodyLink body_link;
};


extern Object bodyObject(BodyLink body_link);
