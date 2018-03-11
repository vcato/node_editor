#ifndef SCENEVIEWER_HPP_
#define SCENEVIEWER_HPP_

#include "scene.hpp"


class SceneViewer {
  public:
    void notifySceneChanged() { redrawScene(); }
    void setScenePtr(Scene *arg);

  protected:
    Scene *scene_ptr = nullptr;
    const Scene *scenePtr() { return scene_ptr; }

  private:
    virtual void redrawScene() = 0;
};

#endif /* SCENEVIEWER_HPP_ */
