#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>
#include <set>
#include "point2d.hpp"


class Scene {
  public:
    struct Body {
      Point2D position;
    };

    using Bodies = std::vector<Body>;

    struct Observer {
      ~Observer();

      void setScenePtr(Scene *arg);
      Scene *scenePtr() const { return scene_ptr; }

      virtual void sceneChanged() = 0;

      private:
	Scene *scene_ptr = 0;
    };

    ~Scene();

    int nBodies() const { return bodies_member.size(); }
    int nObservers() const { return observers.size(); }
    void notifyChanged();

    void addBody();
    const Bodies &bodies() const { return bodies_member; }
    Bodies &bodies() { return bodies_member; }

  private:
    std::set<Observer *> observers;
    Bodies bodies_member;
};

#endif /* SCENE_HPP_ */
