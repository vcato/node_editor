#include "sceneobjects.hpp"

#include "maybepoint2d.hpp"
#include "globalvec.hpp"


using std::cerr;
using std::vector;


Object bodyObject(BodyLink body_link)
{
  return Object(*new BodyObjectData(body_link));
}


Optional<Any> BodyObjectData::member(const std::string &member_name)
{
  if (member_name=="globalVec") {
#if 0
    return {
      Function(
        [body_link=body_link](const vector<Any> &parameters) {
          if (parameters.size()!=1) {
            assert(false);
          }
          Optional<Point2D> maybe_point2d = maybePoint2D(parameters[0]);

          if (!maybe_point2d) {
            assert(false);
          }

          return globalVec(body_link,*maybe_point2d - Point2D(0,0));
        }
      )
    };
#else
    assert(false);
#endif
  }

  cerr << "member_name: " << member_name << "\n";
  assert(false);
}
