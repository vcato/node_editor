#include <QDialog>
#include <QTreeWidget>
#include "scene.hpp"
#include "qtsceneviewer.hpp"
#include "qtscenetree.hpp"
#include "scenewindow.hpp"


class QtSceneWindow : public QDialog, public SceneWindow {
  public:
    QtSceneWindow(QWidget *parent_widget_ptr);

    QtSceneViewer &viewer() override;
    QtSceneTree &tree() override;
    void setTitle(const std::string &) override;

  private:
    Scene *scene_ptr;
    QtSceneViewer *viewer_ptr;
    QtSceneTree *tree_ptr;
};
