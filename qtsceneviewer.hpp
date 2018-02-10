#include <QGLWidget>
#include "scene.hpp"
#include "sceneviewer.hpp"


class QtSceneViewer : public QGLWidget, public SceneViewer {
  public:
    QtSceneViewer();
    ~QtSceneViewer();

    void paintGL();
    void setScenePtr(Scene *);

  private:
    Scene *scene_ptr = nullptr;
    const Scene *scenePtr();

    virtual void sceneChanged();
    QSize sizeHint() const override { return QSize(640,480); }

};
