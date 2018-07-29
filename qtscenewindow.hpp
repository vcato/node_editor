#include <QDialog>
#include "scene.hpp"
#include "scenewindow.hpp"


class QtSceneTree;
class QtSceneViewer;


class QtSceneWindow : public QDialog, public SceneWindow {
  public:
    QtSceneWindow(QWidget *parent_widget_ptr);

  private:
    Scene *scene_ptr;
    QtSceneViewer *viewer_ptr;
    QtSceneTree *tree_ptr;

    SceneTree &tree() override;
    void setTitle(const std::string &) override;
    SceneViewer &viewer() override;
};
