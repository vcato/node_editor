#ifndef SCENEVIEWER_HPP_
#define SCENEVIEWER_HPP_

class SceneViewer {
  public:
    void notifySceneChanged() { sceneChanged(); }

  private:
    virtual void sceneChanged() = 0;
};

#endif /* SCENEVIEWER_HPP_ */
