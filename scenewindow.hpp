#ifndef SCENEWINDOW_HPP_
#define SCENEWINDOW_HPP_

#include "sceneviewer.hpp"
#include "scenetree.hpp"


class SceneWindow {
  public:
    void notifySceneChanged();
    void notifyBodyAdded(const Scene::Body &);
    void setScenePtr(Scene *,const std::string &name);
    void setDisplayFrame(const Scene::Frame &);

  private:
    virtual SceneViewer &viewer() = 0;
    virtual SceneTree &tree() = 0;
    virtual void setTitle(const std::string &) = 0;
};

#endif /* SCENEWINDOW_HPP_ */
