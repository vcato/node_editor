#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <vector>
#include <set>


class Scene {
  public:
    struct Body {
    };

    using Bodies = std::vector<Body>;

    struct Observer {
      Observer() : scene_ptr(nullptr) { }
      ~Observer();

      void setScenePtr(Scene *arg);

      virtual void sceneChanged() = 0;

      private:
	Scene *scene_ptr;
    };

    ~Scene();

    int nBodies() const { return bodies_member.size(); }
    int nObservers() const { return observers.size(); }

    void addBody();
    const Bodies &bodies() const { return bodies_member; }
    Bodies &bodies() { return bodies_member; }

  private:
    std::set<Observer *> observers;
    Bodies bodies_member;
};

#endif /* SCENE_HPP_ */
