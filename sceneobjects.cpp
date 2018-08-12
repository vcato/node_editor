#include "sceneobjects.hpp"


using std::cerr;


Object bodyObject(BodyLink body_link)
{
  return Object(*new BodyObjectData(body_link));
}


Optional<Any> BodyObjectData::member(const std::string &member_name)
{
  cerr << "member_name: " << member_name << "\n";
  assert(false);
}
