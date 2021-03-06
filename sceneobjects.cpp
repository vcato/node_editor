#include "sceneobjects.hpp"

#include "maybepoint2d.hpp"
#include "globalvec.hpp"
#include "point2dobject.hpp"


using std::cerr;
using std::vector;
using std::make_unique;


Any SceneObjectData::member(const std::string &member_name) const
{
  auto n_bodies = scene.nBodies();

  for (auto i=n_bodies*0; i!=n_bodies; ++i) {
    Scene::Body &body = scene.body(i);

    // This is where we check to see if we have a body in the scene.
    if (body.name==member_name) {
      return makeBodyObject(BodyLink(&scene,&body));
    }
  }

  assert(false);
}


std::vector<std::string> BodyObjectData::memberNames() const
{
  return {"globalVec","pos"};
}


static Any makeVector(const Point2D &result_value)
{
  vector<Any> result;
  result.push_back(result_value.x);
  result.push_back(result_value.y);
  return Any(std::move(result));
}


static Any
  bodyPosFunction(const BodyLink &body_link,const vector<Any> &parameters)
{
  int n_parameters = parameters.size();

  if (n_parameters==0) {
    Point2D result =
      globalPos(
        body_link.body(),
        /*local*/Point2D(0,0),
        body_link.scene().displayFrame()
      );
    return makePoint2DObject(result);
  }

  // We have some parameters.  The only thing that is supported
  // so far is a single parameter that is a vector representing
  // a point.

  if (parameters.size()!=1) {
    assert(false);
  }

  Optional<Point2D> maybe_point2d = maybePoint2D(parameters[0]);

  if (!maybe_point2d) {
    assert(false);
  }

  const Point2D &local = *maybe_point2d;

  if (!body_link.hasValue()) {
    return makeVector(local);
  }

  const Scene::Frame &frame = body_link.scene().displayFrame();
  const Scene::Body &body = body_link.body();
  Point2D result_value = globalPos(body,local,frame);
  return makeVector(result_value);
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
    return {
      Function{
        [body_link=body_link](const vector<Any> &parameters) -> Optional<Any> {
          return bodyPosFunction(body_link,parameters);
        }
      }
    };
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
