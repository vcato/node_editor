#ifndef SCENEVIEWER_HPP_
#define SCENEVIEWER_HPP_

#include "viewportcoords.hpp"
#include "scene.hpp"
#include "optional.hpp"


struct SceneListener {
  virtual void
    frameVariablesChanged(
      int frame_index,
      std::vector<int> &variable_indices
    ) = 0;
};


class SceneViewer {
  public:
    void notifySceneChanged() { redrawScene(); }
    void setScenePtr(Scene *arg,SceneListener *);

  protected:
    Scene *scene_ptr = nullptr;
    SceneListener *listener_ptr = nullptr;
    const Scene *scenePtr() { return scene_ptr; }
    void mousePressedAt(const ViewportPoint &p);
    void mouseMovedTo(const ViewportPoint &p);
    void mouseReleased();

  private:
    virtual void redrawScene() = 0;

    Scene::Body *clicked_on_body_ptr = nullptr;
    Optional<ViewportPoint> maybe_mouse_click_down_position;
    Optional<Point2D> maybe_body_click_down_position;
};

#endif /* SCENEVIEWER_HPP_ */
