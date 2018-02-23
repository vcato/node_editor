#ifndef SCENETREE_HPP_
#define SCENETREE_HPP_

#include "scene.hpp"


class SceneTree {
  public:
    struct Item {
      std::string label;
      using Children = std::vector<Item>;
      Children children;
    };

    void notifySceneChanged();
    void setScenePtr(Scene *);

  private:
    Scene *scene_ptr = 0;

    virtual void setItems(const Item &root) = 0;
    void updateItems();
};

#endif /* SCENETREE_HPP_ */
