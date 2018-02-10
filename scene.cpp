#include "scene.hpp"

#include <cassert>
#include <iostream>

using std::cerr;


Scene::~Scene()
{
}


void Scene::addBody()
{
  bodies_member.push_back(Body());
}
