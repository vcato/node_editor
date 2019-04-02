#include "sceneviewer.hpp"


struct FakeSceneViewer : SceneViewer {
  int redraw_count = 0;

  void redrawScene() override
  {
    ++redraw_count;
  }

  void userPressesMouseAt(const ViewportPoint &p)
  {
    mousePressedAt(p);
  }

  void userMovesMouseTo(const ViewportPoint &p)
  {
    mouseMovedTo(p);
  }

  void userReleasesMouse()
  {
    mouseReleased();
  }

  ViewportPoint centerOfBody(const Scene::Body &);
};
