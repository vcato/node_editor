#include <QDialog>
#include "scene.hpp"
#include "qtsceneviewer.hpp"


class QtSceneWindow : public QDialog {
  public:
    QtSceneWindow(QWidget *parent_widget_ptr);
    void setScenePtr(Scene *);
    QtSceneViewer &viewer();

  private:
    Scene *scene_ptr;
    QtSceneViewer *viewer_ptr;
};
