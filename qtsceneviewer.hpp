#include <QGLWidget>
#include "scene.hpp"
#include "sceneviewer.hpp"


class QtSceneViewer : public QGLWidget, public SceneViewer {
  public:
    QtSceneViewer();
    ~QtSceneViewer();

  private:
    void paintGL() override;
    void redrawScene() override;
    QSize sizeHint() const override { return QSize(640,480); }
};
