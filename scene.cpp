#include "scene.hpp"

#include <cassert>
#include <iostream>

using std::cerr;
using std::string;
using std::vector;


Scene::~Scene()
{
}


string Scene::newBodyName() const
{
  string name;

  {
    int body_number = 1;
    for (;;) {
      name = "Body" + std::to_string(body_number);

      if (!hasBody(name)) {
	break;
      }

      ++body_number;
    }
  }

  return name;
}


auto Scene::addBody() -> Body &
{
  string name = newBodyName();

  Body& new_body = createChild(bodies_member);
  new_body.name = name;
  return new_body;
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


void Scene::addChildBodyTo(Body &parent)
{
  string name = newBodyName();
  Body &child = parent.addChild();
  child.name = name;
}


Scene::Bodies::Bodies(const Bodies &arg)
{
  for (auto& body : arg) {
    createChild(body);
  }
}
