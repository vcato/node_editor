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


Scene::Scene()
{
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
  background_frame.setNVariables(n_frame_variables);
  display_frame.setNVariables(n_frame_variables);
}


Point2DMap Scene::newPositionMap()
{
  VarIndex x_var = n_frame_variables;
  VarIndex y_var = n_frame_variables + 1;
  addVars(2);
  return Point2DMap{x_var,y_var};
}


auto Scene::addBody() -> Body &
{
  string name = newBodyName();
  Point2DMap position_map = newPositionMap();
  Body& new_body = bodies_member.createChild(position_map);
  new_body.name = name;
  return new_body;
}


Body& Scene::addChildBodyTo(Body &parent)
{
  return parent.addChild(newBodyName(),newPositionMap());
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
