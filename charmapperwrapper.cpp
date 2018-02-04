#include "charmapperwrapper.hpp"

#include <iostream>
#include "worldpolicies.hpp"

using std::cerr;
using OperationVisitor = TreeItem::OperationVisitor;


static TreeItem motionPassItem()
{
  return TreeItem(world_policies::MotionPassPolicy{});
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


namespace {
struct MotionPassWrapper : Wrapper {
  using MotionPass = Charmapper::MotionPass;
  using Position = Charmapper::Position;
  MotionPass &motion_pass;
  using PosExpr = MotionPass::PosExpr;
  using Channel = Charmapper::Channel;
  using GlobalPosition = Charmapper::GlobalPosition;
  using FromBodyGlobalPosition = Charmapper::FromBodyGlobalPosition;
  using FromComponentsGlobalPosition =
    Charmapper::FromComponentsGlobalPosition;

  struct ChannelWrapper : Wrapper {
    Channel &channel;

    ChannelWrapper(Channel &channel_arg)
    : channel(channel_arg)
    {
    }

    virtual void
      visitOperations(
        const TreePath &,
        const TreeItem::OperationVisitor &
      ) const
    {
    }

    virtual Diagram *diagramPtr() const
    {
      return &channel.diagram;
    }

    void
      visitChildWrapper(
        const TreePath &/*path*/,
        int /*depth*/,
        const WrapperVisitor &/*visitor*/
      ) const
    {
      assert(false);
    }
  };

  struct PositionWrapper : Wrapper {
    Position &position;

    virtual void
      visitOperations(
        const TreePath &,
        const TreeItem::OperationVisitor &
      ) const
    {
    }

    virtual Diagram *diagramPtr() const
    {
      return &position.diagram;
    }

    PositionWrapper(Position &position_arg)
    : position(position_arg)
    {
    }

    void
      visitChildWrapper(
        const TreePath &path,
        int depth,
        const WrapperVisitor &visitor
      ) const
    {
      int child_index = path[depth];

      switch (child_index) {
        case 0:
          ChannelWrapper(position.x).visitWrapper(path,depth+1,visitor);
          return;
        case 1:
          ChannelWrapper(position.y).visitWrapper(path,depth+1,visitor);
          return;
        case 2:
          ChannelWrapper(position.z).visitWrapper(path,depth+1,visitor);
          return;
      }

      // assert(false);
    }
  };

  struct FromBodyGlobalPositionWrapper : Wrapper {
    FromBodyGlobalPosition &from_body_global_position;

    FromBodyGlobalPositionWrapper(FromBodyGlobalPosition &arg)
    : from_body_global_position(arg)
    {
    }

    void
      visitChildWrapper(
        const TreePath &/*path*/,
        int /*depth*/,
        const WrapperVisitor &/*visitor*/
      ) const
    {
      assert(false);
    }

    virtual void
      visitOperations(
        const TreePath &,
        const TreeItem::OperationVisitor &
      ) const
    {
      assert(false);
    }

    virtual Diagram *diagramPtr() const
    {
      assert(false);
    }
  };

  struct GlobalPositionWrapper : Wrapper {
    GlobalPosition &global_position;

    GlobalPositionWrapper(GlobalPosition &arg)
    : global_position(arg)
    {
    }

    struct TypeVisitor : GlobalPosition::Visitor {
      const TreePath &path;
      const int depth;
      const WrapperVisitor &visitor;

      TypeVisitor(
        const TreePath &path_arg,
        int depth_arg,
        const WrapperVisitor &visitor_arg
      )
      : path(path_arg), depth(depth_arg), visitor(visitor_arg)
      {
      }

      virtual void accept(FromBodyGlobalPosition &arg) const
      {
        FromBodyGlobalPositionWrapper(arg).visitWrapper(path,depth,visitor);
      }

      virtual void accept(FromComponentsGlobalPosition &arg) const
      {
        PositionWrapper(arg)
          .visitWrapper(path,depth,visitor);
      }
    };

    void
      visitChildWrapper(
        const TreePath &/*path*/,
        int /*depth*/,
        const WrapperVisitor &/*visitor*/
      ) const
    {
      // This isn't working properly yet, since charmapper doesn't
      // actually change the type of the global position between
      // from-component and from-body when it is changed in the tree.
      // global_position.accept(TypeVisitor(path,depth,visitor));
    }

    virtual void
      visitOperations(
        const TreePath &,
        const TreeItem::OperationVisitor &
      ) const
    {
      assert(false);
    }


    virtual Diagram *diagramPtr() const
    {
      return 0;
    }
  };

  struct PosExprWrapper : Wrapper {
    PosExpr &pos_expr;

    PosExprWrapper(PosExpr &pos_expr_arg)
    : pos_expr(pos_expr_arg)
    {
    }

    virtual void
      visitOperations(
        const TreePath &,
        const TreeItem::OperationVisitor &
      ) const
    {
    }

    virtual Diagram *diagramPtr() const
    {
      return &pos_expr.diagram;
    }

    void
      visitChildWrapper(
        const TreePath &path,
        int depth,
        const WrapperVisitor &visitor
      ) const
    {
      int child_index = path[depth];

      cerr << "child_index: " << child_index << "\n";
      // assert(false);

      if (child_index==0) {
        // Target body
      }
      else if (child_index==1) {
        PositionWrapper(pos_expr.local_position).visitWrapper(
          path,depth+1,visitor
        );
      }
      else if (child_index==2) {
        assert(pos_expr.global_position_ptr);
        GlobalPositionWrapper(*pos_expr.global_position_ptr).visitWrapper(
          path,depth+1,visitor
        );
      }
    }
  };

  MotionPassWrapper(Charmapper::MotionPass &motion_pass_arg)
  : motion_pass(motion_pass_arg)
  {
  }

  virtual void
    visitOperations(const TreePath &path,const OperationVisitor &visitor) const
  {
    Charmapper::MotionPass &motion_pass = this->motion_pass;
    visitor(
      "Add Pos Expr",
      [path,&motion_pass](TreeOperationHandler &handler){
      motion_pass.addPosExpr();
      handler.addItem(path,posExprItem());
      }
    );
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void
    visitChildWrapper(
      const TreePath &path,
      int depth,
      const WrapperVisitor &visitor
    ) const
  {
    assert(motion_pass.pos_exprs[path[depth]]);
    PosExpr &pos_expr = *motion_pass.pos_exprs[path[depth]];
    PosExprWrapper(pos_expr).visitWrapper(path,depth+1,visitor);
  }
};
}


void
  CharmapperWrapper::visitOperations(
    const TreePath &path,
    const OperationVisitor &visitor
  ) const
{
  Charmapper &charmapper = this->charmapper;
  visitor(
    "Add Motion Pass",
    [path,&charmapper](TreeOperationHandler &handler){
      charmapper.addMotionPass();
      handler.addItem(path,motionPassItem());
    }
  );
}


void
  CharmapperWrapper::visitChildWrapper(
    const TreePath &path,
    int depth,
    const WrapperVisitor &visitor
  ) const
{
  int child_index = path[depth];
  assert(charmapper.passes[child_index]);

  MotionPassWrapper child_wrapper{*charmapper.passes[child_index]};
  return child_wrapper.visitWrapper(path,depth+1,visitor);
}
