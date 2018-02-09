#include <QGLWidget>
#include "scene.hpp"


class QtSceneViewer : public QGLWidget {
  public:
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

    void paintGL();
    void setScenePtr(Scene *);

  private:
    SceneObserver scene_observer;
    const Scene *scenePtr();

    void sceneChanged();
    QSize sizeHint() const override { return QSize(640,480); }

};
