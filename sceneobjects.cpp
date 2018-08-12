#include "sceneobjects.hpp"


Object bodyObject(BodyLink body_link)
{
  return Object(*new BodyObjectData(body_link));
}
