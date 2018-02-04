#include "world.hpp"

#include "worldpolicies.hpp"


using OperationVisitor = TreeItem::OperationVisitor;
using std::make_unique;


static TreeItem charmapperItem()
{
  return TreeItem(world_policies::CharmapperPolicy{});
}


static TreeItem motionPassItem()
{
  return TreeItem(world_policies::MotionPassPolicy{});
}


static TreeItem sceneItem()
{
  return TreeItem(world_policies::ScenePolicy{});
}


static TreeItem bodyItem()
{
  return TreeItem(world_policies::BodyPolicy{});
}


static TreeItem posExprItem()
{
  TreeItem pos_expr_item(world_policies::PosExprPolicy{});
  pos_expr_item.createItem2(world_policies::TargetBodyPolicy{});
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(world_policies::LocalPositionPolicy{});
    world_policies::createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(world_policies::GlobalPositionPolicy{});
    world_policies::createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }

  return pos_expr_item;
}


struct World::WorldMember {
  virtual bool
    visitOperations(
      const TreePath &path,int depth,const OperationVisitor &visitor
    ) = 0;
};


namespace {
struct CharmapperObject : World::WorldMember {
  Charmapper charmapper;

  struct MotionPassWrapper {
    Charmapper::MotionPass &motion_pass;

    bool
      visitOperations(
        const TreePath &path,
        int depth,
        const OperationVisitor &visitor
      )
    {
      int path_length = path.size();

      if (depth==path_length) {
        visitor(
          "Add Pos Expr",
          [path](TreeOperationHandler &handler){
            handler.addItem(path,posExprItem());
          }
        );

        return true;
      }

      return false;
    }
  };

  virtual bool
    visitOperations(
      const TreePath &path,
      int depth,
      const OperationVisitor &visitor
    )
  {
    int path_length = path.size();

    if (depth==path_length) {
      visitor(
        "Add Motion Pass",
        [path,this](TreeOperationHandler &handler){
          charmapper.addMotionPass();
          handler.addItem(path,motionPassItem());
        }
      );

      return true;
    }

    int child_index = path[depth];

    MotionPassWrapper child_wrapper{charmapper.passes[child_index]};
    return child_wrapper.visitOperations(path,depth+1,visitor);
  }
};
}


namespace {
struct SceneObject : World::WorldMember {
  Scene scene;

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

  virtual bool
    visitOperations(
      const TreePath &path,
      int depth,
      const OperationVisitor &visitor
    )
  {
    int path_length = path.size();

    if (depth==path_length) {
      visitor(
        "Add Body",
        [path,this](TreeOperationHandler &handler){
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
};
}


World::World() = default;
World::~World() = default;


bool
  World::visitOperations(
    const TreePath &path,int depth,const OperationVisitor &visitor
  )
{
  int path_length = path.size();

  if (depth==path_length) {
    visitor(
      "Add Charmapper",
      [path,this](TreeOperationHandler &handler){
        addCharmapper();
        handler.addItem(path,charmapperItem());
      }
    );
    visitor(
      "Add Scene",
      [path,this](TreeOperationHandler &handler){
        addScene();
        handler.addItem(path,sceneItem());
      }
    );

    return true;
  }

  int child_index = path[depth];

  assert(world_members[child_index]);
  WorldMember &child = *world_members[child_index];
  return child.visitOperations(path,depth+1,visitor);
}


void World::addCharmapper()
{
  world_members.push_back(make_unique<CharmapperObject>());
}


void World::addScene()
{
  world_members.push_back(make_unique<SceneObject>());
  createSceneWindow();
}
