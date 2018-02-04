#include "scenewrapper.hpp"

#include "worldpolicies.hpp"


using OperationVisitor = TreeItem::OperationVisitor;


static TreeItem bodyItem()
{
  return TreeItem(world_policies::BodyPolicy{});
}


namespace {
struct BodyWrapper {
  Scene::Body &body;

  bool
    visitOperations(
      const TreePath &,
      int /*depth*/,
      const OperationVisitor &/*visitor*/
    )
  {
    return false;
  }
};
}


bool
  SceneWrapper::visitOperations(
    const TreePath &path,int depth,const OperationVisitor &visitor
  )
{
  int path_length = path.size();

  if (depth==path_length) {
    Scene &scene = this->scene;
    visitor(
      "Add Body",
      [path,&scene](TreeOperationHandler &handler){
        scene.addBody();
        handler.addItem(path,bodyItem());
      }
    );

    return true;
  }

  return
    BodyWrapper{
      scene.bodies[path[depth]]
    }.visitOperations(path,depth+1,visitor);
}
