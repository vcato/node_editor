#include "charmapperwrapper.hpp"

#include <iostream>
#include "streamvector.hpp"

using std::cerr;
using std::vector;
using std::string;
using SceneList = CharmapperWrapper::SceneList;


namespace {
struct MotionPassWrapper : VoidWrapper {
  using MotionPass = Charmapper::MotionPass;
  using Position = Charmapper::Position;
  using PosExpr = MotionPass::PosExpr;
  using Channel = Charmapper::Channel;
  using GlobalPositionData = Charmapper::GlobalPosition::Data;
  using GlobalPosition = Charmapper::GlobalPosition;
  using FromBodyGlobalPositionData = Charmapper::GlobalPosition::FromBodyData;
  using ComponentsGlobalPositionData =
    Charmapper::GlobalPosition::ComponentsData;

  MotionPass &motion_pass;
  const SceneList &scene_list;

  struct ChannelWrapper : NumericWrapper {
    Channel &channel;
    const char *label_member;

    ChannelWrapper(Channel &channel_arg,const char *label_arg)
    : channel(channel_arg),
      label_member(label_arg)
    {
    }

    void
      withOperations(
        const TreePath &,
        const OperationVisitor &
      ) const override
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

    virtual std::string label() const
    {
      return label_member;
    }

    virtual int nChildren() const
    {
      return 0;
    }

    virtual void setValue(int) const
    {
    }
  };

  struct PositionWrapper : VoidWrapper {
    Position &position;
    const char *label_member;

    PositionWrapper(Position &position_arg,const char *label_arg)
    : position(position_arg),
      label_member(label_arg)
    {
    }

    void
      withOperations(
        const TreePath &path,
        const OperationVisitor &
      ) const override
    {
      cerr << "PositionWrapper::visitOperations: path=" << path << "\n";
    }

    virtual Diagram *diagramPtr() const
    {
      return &position.diagram;
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

    virtual std::string label() const
    {
      return label_member;
    }

    virtual int nChildren() const
    {
      return 3;
    }
  };

  struct FromBodyGlobalPositionWrapper : VoidWrapper {
    FromBodyGlobalPositionData &from_body_global_position;
    const SceneList &scene_list;

    FromBodyGlobalPositionWrapper(
      FromBodyGlobalPositionData &arg,
      const SceneList &scene_list_arg
    )
    : from_body_global_position(arg),
      scene_list(scene_list_arg)
    {
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      if (child_index==0) {
        visitor(SourceBodyWrapper(scene_list));
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

    void
      withOperations(
        const TreePath &,
        const OperationVisitor &
      ) const override
    {
      assert(false);
    }

    virtual Diagram *diagramPtr() const
    {
      assert(false);
    }

    virtual std::string label() const
    {
      assert(false);
    }

    virtual int nChildren() const
    {
      return 2;
    }
  };

  struct GlobalPositionWrapper : EnumerationWrapper {
    GlobalPosition &global_position;
    const SceneList &scene_list;

    GlobalPositionWrapper(
      GlobalPosition &global_position_arg,
      const SceneList &scene_list_arg
    )
    : global_position(global_position_arg),
      scene_list(scene_list_arg)
    {
    }

    struct ValueVisitor : GlobalPositionData::Visitor {
      const WrapperVisitor &visitor;
      const SceneList &scene_list;

      ValueVisitor(
        const WrapperVisitor &visitor_arg,
        const SceneList &scene_list_arg
      )
      : visitor(visitor_arg),
        scene_list(scene_list_arg)
      {
      }

      virtual void visit(FromBodyGlobalPositionData &data) const
      {
        visitor(FromBodyGlobalPositionWrapper(data,scene_list));
      }

      virtual void visit(ComponentsGlobalPositionData &data) const
      {
        visitor(PositionWrapper(data,"blah"));
      }
    };

    void withValueWrapper(const WrapperVisitor &visitor) const
    {
      assert(global_position.global_position_ptr);
      global_position.global_position_ptr->accept(
        ValueVisitor(visitor,scene_list)
      );
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

    void
      withOperations(
        const TreePath &,
        const OperationVisitor &
      ) const override
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

    vector<string> enumerationNames() const override
    {
      return {"Components","From Body"};
    }

    virtual std::string label() const
    {
      return "Global Position";
    }
  };

  struct BodyWrapper : EnumerationWrapper {
    const SceneList &scene_list;

    BodyWrapper(const SceneList &scene_list_arg)
    : scene_list(scene_list_arg)
    {
    }

    void
      withOperations(
        const TreePath &,
        const OperationVisitor &
      ) const override
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

    void
      comboBoxItemIndexChanged(
        const TreePath &,
        int /*index*/,
        OperationHandler &
      ) const override
    {
    }

    int nChildren() const override
    {
      return 0;
    }

    void
      handleSceneChange(
        const Wrapper::OperationHandler &operation_handler,
        const TreePath &path_of_this
      ) const
    {
      operation_handler.changeEnumerationValues(path_of_this);
    }

    vector<string> enumerationNames() const override
    {
      return scene_list.allBodyNames();
    }
  };

  struct TargetBodyWrapper : BodyWrapper {
    using BodyWrapper::BodyWrapper;

    std::string label() const override { return "Target Body"; }
  };

  struct SourceBodyWrapper : BodyWrapper {
    using BodyWrapper::BodyWrapper;

    std::string label() const override { return "Source Body"; }
  };


  struct PosExprWrapper : VoidWrapper {
    PosExpr &pos_expr;
    const SceneList &scene_list;

    PosExprWrapper(PosExpr &pos_expr_arg,const SceneList &scene_list_arg)
    : pos_expr(pos_expr_arg),
      scene_list(scene_list_arg)
    {
    }

    void
      withOperations(
        const TreePath &,
        const OperationVisitor &
      ) const override
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
        visitor(TargetBodyWrapper(scene_list));
      }
      else if (child_index==1) {
        visitor(PositionWrapper(pos_expr.local_position,"Local Position"));
      }
      else if (child_index==2) {
        visitor(GlobalPositionWrapper(pos_expr.global_position,scene_list));
      }
      else {
        assert(false);
      }
    }

    virtual std::string label() const
    {
      return "Pos Expr";
    }

    virtual int nChildren() const
    {
      return 3;
    }
  };

  MotionPassWrapper(
    Charmapper::MotionPass &motion_pass_arg,
    const SceneList &scene_list_arg
  )
  : motion_pass(motion_pass_arg), scene_list(scene_list_arg)
  {
  }

  void
    withOperations(
      const TreePath &path,
      const OperationVisitor &visitor
    ) const override
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
    visitor(PosExprWrapper(pos_expr,scene_list));
  }

  virtual std::string label() const
  {
    return "Motion Pass";
  }

  virtual int nChildren() const
  {
    return motion_pass.pos_exprs.size();
  }
};
}


void
  CharmapperWrapper::withOperations(
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
  visitor(MotionPassWrapper{*charmapper.passes[child_index],scene_list});
}


static void
  forEachSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path_of_wrapper,
    const std::function<void(const Wrapper &,const TreePath &)> &f
  )
{
  f(wrapper,path_of_wrapper);
  int n_children = wrapper.nChildren();

  for (int i=0; i!=n_children; ++i) {
    wrapper.withChildWrapper(
      i,[&](const Wrapper &child_wrapper){
        forEachSubWrapper(child_wrapper,join(path_of_wrapper,i),f);
      }
    );
  }
}


void
  CharmapperWrapper::handleSceneChange(
    const OperationHandler &operation_handler,
    const TreePath &path_of_this
  )
{
  forEachSubWrapper(
    *this,
    path_of_this,
    [&](const Wrapper &sub_wrapper,const TreePath &path_of_sub_wrapper)
    {
      typedef MotionPassWrapper::BodyWrapper BodyWrapper;

      const BodyWrapper *body_wrapper_ptr =
        dynamic_cast<const BodyWrapper*>(&sub_wrapper);

      if (body_wrapper_ptr) {
        body_wrapper_ptr->handleSceneChange(
          operation_handler,path_of_sub_wrapper
        );
      }
    }
  );
}
