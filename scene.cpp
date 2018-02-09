#include "scene.hpp"

#include <cassert>
#include <iostream>

using std::cerr;


void Scene::Observer::setScenePtr(Scene *arg)
{
  if (scene_ptr) {
    scene_ptr->removeObserver(*this);
  }

  scene_ptr = arg;

  if (scene_ptr) {
    scene_ptr->addObserver(*this);
  }
}


Scene::Observer::~Observer()
{
  if (scene_ptr) {
    scene_ptr->removeObserver(*this);
  }
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

  for (Observer *observer_ptr : observers) {
    assert(observer_ptr);
    observer_ptr->sceneChanged();
  }
}


void Scene::removeObserver(Observer &observer)
{
  assert(observers.count(&observer));
  observers.erase(&observer);
}
