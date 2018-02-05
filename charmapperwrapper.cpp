#include "charmapperwrapper.hpp"

#include <iostream>
#include "streamvector.hpp"

using std::cerr;
using std::vector;
using std::string;
using OperationVisitor = TreeItem::OperationVisitor;


namespace {
struct MotionPassWrapper : SimpleWrapper {
  using MotionPass = Charmapper::MotionPass;
  using Position = Charmapper::Position;
  MotionPass &motion_pass;
  using PosExpr = MotionPass::PosExpr;
  using Channel = Charmapper::Channel;
  using GlobalPositionData = Charmapper::GlobalPosition::Data;
  using GlobalPosition = Charmapper::GlobalPosition;
  using FromBodyGlobalPositionData = Charmapper::GlobalPosition::FromBodyData;
  using ComponentsGlobalPositionData =
    Charmapper::GlobalPosition::ComponentsData;

  struct ChannelWrapper : SimpleWrapper {
    Channel &channel;
    const char *label;

    ChannelWrapper(Channel &channel_arg,const char *label_arg)
    : channel(channel_arg),
      label(label_arg)
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

    void withChildWrapper(int /*child_index*/,const WrapperVisitor &) const
    {
      assert(false);
    }

    virtual void visitType(const TreeItem::TypeVisitor &visitor) const
    {
      visitor.numericItem(label);
    }

    virtual int nChildren() const
    {
      return 0;
    }
  };

  struct PositionWrapper : SimpleWrapper {
    Position &position;
    const char *label;

    virtual void
      visitOperations(
        const TreePath &path,
        const TreeItem::OperationVisitor &
      ) const
    {
      cerr << "PositionWrapper::visitOperations: path=" << path << "\n";
    }

    virtual Diagram *diagramPtr() const
    {
      return &position.diagram;
    }

    PositionWrapper(Position &position_arg,const char *label_arg)
    : position(position_arg),
      label(label_arg)
    {
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      switch (child_index) {
        case 0:
          visitor(ChannelWrapper(position.x,"X"));
          return;
        case 1:
          visitor(ChannelWrapper(position.y,"Y"));
          return;
        case 2:
          visitor(ChannelWrapper(position.z,"Z"));
          return;
      }
    }

    virtual void visitType(const TreeItem::TypeVisitor &visitor) const
    {
      cerr << "Visiting position type\n";
      visitor.voidItem(label);
    }

    virtual int nChildren() const
    {
      return 3;
    }
  };

  struct FromBodyGlobalPositionWrapper : SimpleWrapper {
    FromBodyGlobalPositionData &from_body_global_position;

    FromBodyGlobalPositionWrapper(FromBodyGlobalPositionData &arg)
    : from_body_global_position(arg)
    {
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      if (child_index==0) {
        visitor(SourceBodyWrapper());
      }
      else if (child_index==1) {
        visitor(PositionWrapper(
          from_body_global_position.local_position,"Local Position"
        ));
      }
      else {
        cerr << "child_index=" << child_index << '\n';
        assert(false);
      }
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

    virtual void visitType(const TreeItem::TypeVisitor &) const
    {
      assert(false);
    }

    virtual int nChildren() const
    {
      return 2;
    }
  };

  struct GlobalPositionWrapper : Wrapper {
    GlobalPosition &global_position;

    GlobalPositionWrapper(GlobalPosition &arg)
    : global_position(arg)
    {
    }

    struct NChildrenVisitor : GlobalPositionData::Visitor {
      int &n_children;

      NChildrenVisitor(int &n_children_arg)
      : n_children(n_children_arg)
      {
      }

      virtual void accept(FromBodyGlobalPositionData &data) const
      {
        n_children = FromBodyGlobalPositionWrapper(data).nChildren();
      }

      virtual void accept(ComponentsGlobalPositionData &data) const
      {
        n_children = PositionWrapper(data,"blah").nChildren();
      }
    };

    struct ChildWrapperVisitor : GlobalPositionData::Visitor {
      const int child_index;
      const WrapperVisitor &visitor;

      ChildWrapperVisitor(
        int child_index_arg,
        const WrapperVisitor &visitor_arg
      )
      : child_index(child_index_arg),
        visitor(visitor_arg)
      {
      }

      virtual void accept(FromBodyGlobalPositionData &data) const
      {
        FromBodyGlobalPositionWrapper(data).withChildWrapper(
          child_index,visitor
        );
      }

      virtual void accept(ComponentsGlobalPositionData &data) const
      {
        PositionWrapper(data,"blah").withChildWrapper(
          child_index,visitor
        );
      }
    };

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      assert(global_position.global_position_ptr);
      global_position.global_position_ptr->accept(
        ChildWrapperVisitor(child_index,visitor)
      );
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
      return &global_position.diagram;
    }

    virtual void
      comboBoxItemIndexChanged(
        const TreePath &path,
        int index,
        TreeItem::OperationHandler &operation_handler
      ) const
    {
      switch (index) {
        case 0:
          // Components
          {
            global_position.switchToComponents();
            operation_handler.replaceTreeItems(path);
          }
          break;
        case 1:
          // From Body
          {
            global_position.switchToFromBody();
            operation_handler.replaceTreeItems(path);
          }
          break;
        default:
          assert(false);
      }
    }

    virtual void visitType(const TreeItem::TypeVisitor &visitor) const
    {
      vector<string> enumeration_names = {"Components","From Body"};
      visitor.enumeratedItem("Global Position",enumeration_names);
    }

    virtual int nChildren() const
    {
      assert(global_position.global_position_ptr);
      int n_children = 0;
      global_position.global_position_ptr->accept(NChildrenVisitor(n_children));
      return n_children;
    }
  };

  struct TargetBodyWrapper : Wrapper {
    virtual void
      visitOperations(
        const TreePath &,
        const TreeItem::OperationVisitor &
      ) const
    {
    }

    void withChildWrapper(int /*child_index*/,const WrapperVisitor &) const
    {
      assert(false);
    }

    virtual Diagram *diagramPtr() const
    {
      return nullptr;
    }

    virtual void visitType(const TreeItem::TypeVisitor &visitor) const
    {
      vector<string> enumeration_names = {"Body1","Body2","Body3"};
      visitor.enumeratedItem("Target Body",enumeration_names);
    }

    virtual void
      comboBoxItemIndexChanged(
        const TreePath &,
        int /*index*/,
        TreeItem::OperationHandler &
      ) const
    {
    }

    virtual int nChildren() const
    {
      return 0;
    }
  };

  struct SourceBodyWrapper : Wrapper {
    virtual void
      visitOperations(
        const TreePath &,
        const TreeItem::OperationVisitor &
      ) const
    {
    }

    void withChildWrapper(int /*child_index*/,const WrapperVisitor &) const
    {
      assert(false);
    }

    virtual Diagram *diagramPtr() const
    {
      return nullptr;
    }

    virtual void visitType(const TreeItem::TypeVisitor &visitor) const
    {
      vector<string> enumeration_names = {"Body1","Body2","Body3"};
      visitor.enumeratedItem("Source Body",enumeration_names);
    }

    virtual void
      comboBoxItemIndexChanged(
        const TreePath &,
        int /*index*/,
        TreeItem::OperationHandler &
      ) const
    {
    }

    virtual int nChildren() const
    {
      return 0;
    }
  };


  struct PosExprWrapper : SimpleWrapper {
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

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      if (child_index==0) {
        // Target body
        visitor(TargetBodyWrapper());
      }
      else if (child_index==1) {
        visitor(PositionWrapper(pos_expr.local_position,"Local Position"));
      }
      else if (child_index==2) {
        visitor(GlobalPositionWrapper(pos_expr.global_position));
      }
      else {
        assert(false);
      }
    }

    virtual void visitType(const TreeItem::TypeVisitor &visitor) const
    {
      visitor.voidItem("Pos Expr");
    }

    virtual int nChildren() const
    {
      return 3;
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
        handler.addItem(path);
      }
    );
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    PosExpr &pos_expr = *motion_pass.pos_exprs[child_index];
    visitor(PosExprWrapper(pos_expr));
  }

  virtual void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Motion Pass");
  }

  virtual int nChildren() const
  {
    return motion_pass.pos_exprs.size();
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
      handler.addItem(path);
    }
  );
}


void
  CharmapperWrapper::withChildWrapper(
    int child_index,const WrapperVisitor &visitor
  ) const
{
  assert(charmapper.passes[child_index]);

  MotionPassWrapper child_wrapper{*charmapper.passes[child_index]};
  visitor(child_wrapper);
}
