#include "bodylink.hpp"
#include "any.hpp"


struct BodyObjectData : Object::Data {
  BodyObjectData(BodyLink body_link_arg)
  : body_link(body_link_arg)
  {
  }

  Data *clone() override { return new BodyObjectData(*this); }
  std::string typeName() const override { return "Body"; }
  Any member(const std::string &member_name) const override;
  std::vector<std::string> memberNames() const override;

  BodyLink body_link;
};


extern Object makeBodyObject(BodyLink body_link);


struct SceneObjectData : Object::Data {
  SceneObjectData(Scene &scene_arg)
  : scene(scene_arg)
  {
  }

  Data *clone() override { return new SceneObjectData(*this); }

  std::string typeName() const override
  {
    return "Scene";
  }

  Any member(const std::string &member_name) const override;

  std::vector<std::string> memberNames() const override
  {
    std::vector<std::string> names;
    auto n_bodies = scene.nBodies();

    for (auto i=n_bodies*0; i!=n_bodies; ++i) {
      Scene::Body &body = scene.body(i);
      names.push_back(body.name);
    }

    return names;
  }

  Scene &scene;
};


extern Object makeSceneObject(Scene &scene);
