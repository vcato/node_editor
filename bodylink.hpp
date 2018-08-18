#ifndef BODYLINK_HPP
#define BODYLINK_HPP

#include "scene.hpp"


class BodyLink {
  public:
    BodyLink(Scene *scene_ptr_arg,Scene::Body *body_ptr_arg)
    : scene_ptr(scene_ptr_arg),
      body_ptr(body_ptr_arg)
    {
    }

    BodyLink() = default;

    Scene &scene() const
    {
      assert(scene_ptr);
      return *scene_ptr;
    }

    void set(Scene *scene_ptr_arg,Scene::Body *body_ptr_arg)
    {
      assert(!body_ptr_arg || scene_ptr_arg);
      scene_ptr = scene_ptr_arg;
      body_ptr = body_ptr_arg;
    }

    bool hasValue() const
    {
      return body_ptr!=nullptr;
    }

    Scene::Body *bodyPtr() const
    {
      return body_ptr;
    }

    Scene::Body &body() const
    {
      assert(body_ptr);
      return *body_ptr;
    }

    void clear()
    {
      *this = BodyLink();
    }

    bool operator==(const BodyLink &arg) const
    {
      return scene_ptr==arg.scene_ptr && body_ptr==arg.body_ptr;
    }

  private:
    Scene *scene_ptr = nullptr;
    Scene::Body *body_ptr = nullptr;
};

#endif /* BODYLINK_HPP */
