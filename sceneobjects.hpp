#include "bodylink.hpp"
#include "any.hpp"


struct BodyObjectData : Object::Data {
  BodyObjectData(BodyLink body_link_arg)
  : body_link(body_link_arg)
  {
  }

  virtual Data *clone() { return new BodyObjectData(*this); }

  virtual Optional<Any> member(const std::string &/*member_name*/)
  {
    assert(false);
  }

  virtual ~BodyObjectData() {}

  BodyLink body_link;
};


extern Object bodyObject(BodyLink body_link);
