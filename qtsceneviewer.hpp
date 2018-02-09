#include <QGLWidget>
#include "scene.hpp"


struct QtSceneViewer : QGLWidget {
  QtSceneViewer();
  ~QtSceneViewer();

  struct SceneObserver : Scene::Observer {
    QtSceneViewer &viewer;

    SceneObserver(QtSceneViewer &viewer_arg)
    : viewer(viewer_arg)
    {
    }

    virtual void sceneChanged()
    {
      viewer.sceneChanged();
    }
  };

  QSize sizeHint() const override { return QSize(640,480); }

  void paintGL();
  void setScenePtr(Scene *);
  void sceneChanged();

  SceneObserver scene_observer;
};
