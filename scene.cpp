#include "scene.hpp"

#include <cassert>
#include <iostream>

using std::cerr;


void Scene::Observer::setScenePtr(Scene *arg)
{
  if (scene_ptr) {
    assert(scene_ptr->observers.count(this));
    scene_ptr->observers.erase(this);
  }

  scene_ptr = arg;

  if (scene_ptr) {
    scene_ptr->observers.insert(this);
  }
}


Scene::Observer::~Observer()
{
  setScenePtr(nullptr);
}


Scene::~Scene()
{
  while (!observers.empty()) {
    Observer *observer_ptr = *observers.begin();
    assert(observer_ptr);
    observer_ptr->setScenePtr(0);
  }
}


void Scene::addBody()
{
  bodies_member.push_back(Body());
  notifyChanged();
}


void Scene::notifyChanged()
{
  for (Observer *observer_ptr : observers) {
    assert(observer_ptr);
    observer_ptr->sceneChanged();
  }
}
