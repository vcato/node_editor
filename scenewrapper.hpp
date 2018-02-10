#include "scene.hpp"
#include "wrapper.hpp"


struct SceneWrapper : VoidWrapper {
  Scene &scene;

  SceneWrapper(Scene &scene_arg)
  : scene(scene_arg)
  {
  }

  virtual void
    visitOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const;

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int child_index,const WrapperVisitor &) const;

  virtual std::string label() const
  {
    return "Scene";
  }

  virtual int nChildren() const
  {
    return scene.nBodies();
  }
};
