#include "sceneobjects.hpp"

#include "maybepoint2d.hpp"
#include "globalvec.hpp"
#include "point2dobject.hpp"


#define USE_POS_FUNCTION 0


using std::cerr;
using std::vector;
using std::make_unique;


std::vector<std::string> BodyObjectData::memberNames() const
{
  return {"globalVec","pos"};
}


Any BodyObjectData::member(const std::string &member_name) const
{
  if (member_name=="globalVec") {
    return {
      Function{
        [body_link=body_link](const vector<Any> &parameters) -> Optional<Any> {
          if (parameters.size()!=1) {
            assert(false);
          }
          Optional<Point2D> maybe_point2d = maybePoint2D(parameters[0]);

          if (!maybe_point2d) {
            assert(false);
          }

          Vector2D result_value =
            globalVec(body_link,*maybe_point2d - Point2D(0,0));
          vector<Any> result;
          result.push_back(result_value.x);
          result.push_back(result_value.y);
          return Any(std::move(result));
        }
      }
    };
  }

  if (member_name=="pos") {
#if !USE_POS_FUNCTION
    Point2D body_position =
      bodyPosition(body_link.body(),body_link.scene().displayFrame());
    return makePoint2DObject(body_position);
#else
    return {
      Function{
        [body_link=body_link](const vector<Any> &parameters) -> Optional<Any> {
          if (parameters.size()==0) {
            Point2D result = globalPos(body_link,/*local*/Point2D(0,0));;
            return makePoint2DObject(result);
          }

          assert(false);

#if 0
          if (parameters.size()!=1) {
            assert(false);
          }
          Optional<Point2D> maybe_point2d = maybePoint2D(parameters[0]);

          if (!maybe_point2d) {
            assert(false);
          }

          Vector2D result_value =
            globalVec(body_link,*maybe_point2d - Point2D(0,0));
          vector<Any> result;
          result.push_back(result_value.x);
          result.push_back(result_value.y);
          return Any(std::move(result));
#endif
        }
      }
    };
#endif
  }

  cerr << "member_name: " << member_name << "\n";
  assert(false);
}


Object makeBodyObject(BodyLink body_link)
{
  return Object(make_unique<BodyObjectData>(body_link));
}


Object makeSceneObject(Scene &scene)
{
  return Object(make_unique<SceneObjectData>(scene));
}
