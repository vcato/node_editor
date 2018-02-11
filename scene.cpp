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


void Scene::addBody()
{
  string name = newBodyName();

  bodies_member.push_back(Body());
  Body &new_body = bodies_member.back();
  new_body.name = name;
}


bool Scene::hasBody(const vector<Body> &bodies,const std::string &name) const
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
  parent.addChild();
  parent.children.back().name = name;
}
