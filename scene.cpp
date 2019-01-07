#include "scene.hpp"

#include <cassert>
#include <iostream>
#include "generatename.hpp"

using std::cerr;
using std::string;
using std::vector;
using std::function;
using Frame = Scene::Frame;
using Point2DMap = Scene::Point2DMap;
using Body = Scene::Body;


void Scene::Frame::setNVariables(int arg)
{
  var_values.resize(arg,defaultVariableValue());
}


Scene::Scene()
{
  background_motion.frames.resize(1);
}


Scene::~Scene()
{
}


string Scene::newBodyName() const
{
  auto name_exists_function = [this](const string &name){
    return hasBody(name);
  };

  return generateName("Body",name_exists_function);
}


void Scene::addVars(int n_vars)
{
  n_frame_variables += n_vars;

  if (backgroundFrame().nVariables() < n_frame_variables) {
    for (Frame &frame : background_motion.frames) {
      frame.setNVariables(n_frame_variables);
    }
  }

  if (display_frame.nVariables() < n_frame_variables) {
    display_frame.setNVariables(n_frame_variables);
  }
}


Point2DMap Scene::newPositionMap()
{
  // This is a bit tricky.  When we create position maps, we typically
  // want to introduce new variables for the map to reference, but we
  // may have a situation where we're creating a frame with variables,
  // creating the maps, and then setting those maps to refer to those
  // variables.  What we've done is to have a member which tells us
  // which variable to use next.

  VarIndex x_var = n_frame_variables;
  VarIndex y_var = n_frame_variables + 1;
  addVars(2);
  return Point2DMap{x_var,y_var};
}


auto Scene::addBody() -> Body &
{
  return addBody(newBodyName());
}


auto Scene::addBody(const std::string &name) -> Body &
{
  return addBody(name,newPositionMap());
}


Body &Scene::addBody(const std::string &name,const Point2DMap &position_map)
{
  return bodies().createChild(Body(name,position_map,/*parent_ptr*/&root_body));
}


Body& Scene::addChildBodyTo(Body &parent)
{
  return parent.addChild(newBodyName(),newPositionMap());
}


void Scene::removeChildBodyFrom(Body &parent,int child_index)
{
  parent.removeChild(child_index);
}


bool Scene::hasBody(const Bodies &bodies,const std::string &name) const
{
  for (const Body &body : bodies) {
    if (body.name==name) {
      return true;
    }

    if (hasBody(body.children,name)) {
      return true;
    }
  }

  return false;
}


bool Scene::hasBody(const std::string &name) const
{
  return hasBody(bodies(),name);
}


Scene::Bodies::Bodies(const Bodies &arg)
{
  for (auto& body : arg) {
    createChild(body);
  }
}


Frame Scene::makeFrame() const
{
  return Frame(nFrameVariables());
}


void
  setBodyPosition(
    Scene::Body &body,
    Scene::Frame &frame,
    const Point2D &new_position
  )
{
  body.position.x.set(frame,new_position.x);
  body.position.y.set(frame,new_position.y);
}


Point2D bodyPosition(const Scene::Body &body,const Scene::Frame &frame)
{
  auto x = body.position.x(frame);
  auto y = body.position.y(frame);

  return Point2D(x,y);
}


// We have globalPos as part of the scene, but globalVec is separate.  Not
// sure why.
Point2D
  globalPos(
    const Scene::Body &body,
    const Point2D &local,
    const Scene::Frame &frame
  )
{
  if (body.parentPtr()) {
    return
      globalPos(*body.parentPtr(),Point2D(0,0),frame) +
      (bodyPosition(body,frame) - Point2D(0,0)) +
      (local - Point2D(0,0));
  }
  else {
    return bodyPosition(body,frame) + (local - Point2D(0,0));
  }
}
