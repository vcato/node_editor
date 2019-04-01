#ifndef QTSCENEVIEWER_HPP
#define QTSCENEVIEWER_HPP

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
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

    QSize sizeHint() const override { return QSize(640,480); }
};

#endif /* QTSCENEVIEWER_HPP */
