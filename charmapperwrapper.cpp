#include "charmapperwrapper.hpp"

#include <iostream>
#include "streamvector.hpp"

using std::cerr;
using std::vector;
using std::string;


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
        const OperationVisitor &
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

    virtual void visitType(const TypeVisitor &visitor) const
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
        const OperationVisitor &
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

    virtual void visitType(const TypeVisitor &visitor) const
    {
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
        const OperationVisitor &
      ) const
    {
      assert(false);
    }

    virtual Diagram *diagramPtr() const
    {
      assert(false);
    }

    virtual void visitType(const TypeVisitor &) const
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

    struct ValueVisitor : GlobalPositionData::Visitor {
      const WrapperVisitor &visitor;

      ValueVisitor(const WrapperVisitor &visitor_arg)
      : visitor(visitor_arg)
      {
      }

      virtual void visit(FromBodyGlobalPositionData &data) const
      {
        visitor(FromBodyGlobalPositionWrapper(data));
      }

      virtual void visit(ComponentsGlobalPositionData &data) const
      {
        visitor(PositionWrapper(data,"blah"));
      }
    };

    void withValueWrapper(const WrapperVisitor &visitor) const
    {
      assert(global_position.global_position_ptr);
      global_position.global_position_ptr->accept(ValueVisitor(visitor));
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      withValueWrapper(
        [&](const Wrapper &wrapper){
          wrapper.withChildWrapper(child_index,visitor);
        }
      );
    }

    virtual int nChildren() const
    {
      int n_children = 0;
      withValueWrapper(
        [&](const Wrapper &wrapper){
          n_children = wrapper.nChildren();
        }
      );
      return n_children;
    }

    virtual void
      visitOperations(
        const TreePath &,
        const OperationVisitor &
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
        OperationHandler &operation_handler
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

    virtual void visitType(const TypeVisitor &visitor) const
    {
      vector<string> enumeration_names = {"Components","From Body"};
      visitor.enumeratedItem("Global Position",enumeration_names);
    }
  };

  struct TargetBodyWrapper : Wrapper {
    virtual void
      visitOperations(
        const TreePath &,
        const OperationVisitor &
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

    virtual void visitType(const TypeVisitor &visitor) const
    {
      vector<string> enumeration_names = {"Body1","Body2","Body3"};
      visitor.enumeratedItem("Target Body",enumeration_names);
    }

    virtual void
      comboBoxItemIndexChanged(
        const TreePath &,
        int /*index*/,
        OperationHandler &
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
        const OperationVisitor &
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

    virtual void visitType(const TypeVisitor &visitor) const
    {
      vector<string> enumeration_names = {"Body1","Body2","Body3"};
      visitor.enumeratedItem("Source Body",enumeration_names);
    }

    virtual void
      comboBoxItemIndexChanged(
        const TreePath &,
        int /*index*/,
        OperationHandler &
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
        const OperationVisitor &
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

    virtual void visitType(const TypeVisitor &visitor) const
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
	int index = motion_pass.nExprs();
        motion_pass.addPosExpr();
        handler.addItem(join(path,index));
      }
    );
  }

  virtual Diagram *diagramPtr() const { return nullptr; }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    PosExpr &pos_expr = *motion_pass.pos_exprs[child_index];
    visitor(PosExprWrapper(pos_expr));
  }

  virtual void visitType(const TypeVisitor &visitor) const
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
      int index = charmapper.nPasses();
      charmapper.addMotionPass();
      handler.addItem(join(path,index));
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
