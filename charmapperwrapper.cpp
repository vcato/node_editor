#include "charmapperwrapper.hpp"

#include <iostream>
#include "streamvector.hpp"

using std::cerr;
using std::vector;
using std::string;
using Callbacks = CharmapperWrapper::Callbacks;


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
  const Callbacks &callbacks;

  struct ChannelWrapper : NoOperationWrapper<LeafWrapper<NumericWrapper>> {
    Channel &channel;
    const char *label_member;
    const Callbacks &callbacks;

    ChannelWrapper(
      Channel &channel_arg,
      const char *label_arg,
      const Callbacks &callbacks_arg
    )
    : channel(channel_arg),
      label_member(label_arg),
      callbacks(callbacks_arg)
    {
    }

    Diagram *diagramPtr() const override
    {
      return &channel.diagram;
    }

    virtual std::string label() const
    {
      return label_member;
    }

    virtual void setValue(int arg) const
    {
      channel.value = arg;
      callbacks.notifyCharmapChanged();
    }
  };

  struct PositionWrapper : NoOperationWrapper<VoidWrapper> {
    Position &position;
    const char *label_member;
    const Callbacks &callbacks;

    PositionWrapper(
      Position &position_arg,
      const char *label_arg,
      const Callbacks &callbacks_arg
    )
    : position(position_arg),
      label_member(label_arg),
      callbacks(callbacks_arg)
    {
    }

    Diagram *diagramPtr() const override
    {
      return &position.diagram;
    }

    virtual int nChildren() const
    {
      return 2;
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      switch (child_index) {
        case 0:
          visitor(ChannelWrapper(position.x,"X",callbacks));
          return;
        case 1:
          visitor(ChannelWrapper(position.y,"Y",callbacks));
          return;
      }
    }

    virtual std::string label() const
    {
      return label_member;
    }
  };

  struct FromBodyGlobalPositionWrapper : NoOperationWrapper<VoidWrapper> {
    FromBodyGlobalPositionData &from_body_global_position;
    const Callbacks &callbacks;

    FromBodyGlobalPositionWrapper(
      FromBodyGlobalPositionData &arg,
      const Callbacks &callbacks_arg
    )
    : from_body_global_position(arg),
      callbacks(callbacks_arg)
    {
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      if (child_index==0) {
        visitor(
          BodyWrapper(
            "Source Body",
            from_body_global_position.source_body_ptr,
            callbacks
          )
        );
      }
      else if (child_index==1) {
        visitor(PositionWrapper(
          from_body_global_position.local_position,
          "Local Position",
          callbacks
        ));
      }
      else {
        cerr << "child_index=" << child_index << '\n';
        assert(false);
      }
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

  struct GlobalPositionWrapper : NoOperationWrapper<EnumerationWrapper> {
    GlobalPosition &global_position;
    const Callbacks &callbacks;

    GlobalPositionWrapper(
      GlobalPosition &global_position_arg,
      const Callbacks &callbacks_arg
    )
    : global_position(global_position_arg),
      callbacks(callbacks_arg)
    {
    }

    struct ValueVisitor : GlobalPositionData::Visitor {
      const WrapperVisitor &visitor;
      const Callbacks &callbacks;

      ValueVisitor(
        const WrapperVisitor &visitor_arg,
        const Callbacks &callbacks_arg
      )
      : visitor(visitor_arg),
        callbacks(callbacks_arg)
      {
      }

      virtual void visit(FromBodyGlobalPositionData &data) const
      {
        visitor(FromBodyGlobalPositionWrapper(data,callbacks));
      }

      virtual void visit(ComponentsGlobalPositionData &data) const
      {
        visitor(PositionWrapper(data,"blah",callbacks));
      }
    };

    void withValueWrapper(const WrapperVisitor &visitor) const
    {
      assert(global_position.global_position_ptr);
      global_position.global_position_ptr->accept(
        ValueVisitor(visitor,callbacks)
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

    Diagram *diagramPtr() const override
    {
      return &global_position.diagram;
    }

    virtual void
      setValue(
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

  struct BodyWrapper : NoOperationWrapper<LeafWrapper<EnumerationWrapper>> {
    const Callbacks &callbacks;
    Scene::Body *&body_ptr;
    const char *label_member;

    BodyWrapper(
      const char *label_arg,
      Scene::Body *&body_ptr_arg,
      const Callbacks &callbacks_arg
    )
    : callbacks(callbacks_arg),
      body_ptr(body_ptr_arg),
      label_member(label_arg)
    {
    }

    std::string label() const override { return label_member; }

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
      vector<string> result;
      result.push_back("None");
      vector<string> body_names = callbacks.scene_list.allBodyNames();
      result.insert(result.end(),body_names.begin(),body_names.end());
      return result;
    }

    void
      setValue(
        const TreePath &,
        int index,
        OperationHandler &
      ) const override
    {
      assert(index>=0);

      if (index==0) {
        assert(false);
      }

      body_ptr = callbacks.scene_list.allBodyPtrs()[index-1];
      callbacks.notifyCharmapChanged();
    }
  };

  struct PosExprWrapper : NoOperationWrapper<VoidWrapper> {
    PosExpr &pos_expr;
    const Callbacks &callbacks;

    PosExprWrapper(
      PosExpr &pos_expr_arg,
      const Callbacks &callbacks_arg
    )
    : pos_expr(pos_expr_arg),
      callbacks(callbacks_arg)
    {
    }

    Diagram *diagramPtr() const override
    {
      return &pos_expr.diagram;
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      if (child_index==0) {
        // Target body
        visitor(BodyWrapper("Target Body",pos_expr.target_body_ptr,callbacks));
      }
      else if (child_index==1) {
        visitor(
          PositionWrapper(pos_expr.local_position,"Local Position",callbacks)
        );
      }
      else if (child_index==2) {
        visitor(
          GlobalPositionWrapper(pos_expr.global_position,callbacks)
        );
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
    const Callbacks &callbacks_arg
  )
  : motion_pass(motion_pass_arg),
    callbacks(callbacks_arg)
  {
  }

  virtual std::vector<std::string> operationNames() const
  {
    return { "Add Pos Expr" };
  }

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      OperationHandler &handler
    ) const override
  {
    switch (operation_index) {
      case 0:
        {
          int index = motion_pass.nExprs();
          motion_pass.addPosExpr();
          handler.addItem(join(path,index));
        }
        return;
    }

    assert(false);
  }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    PosExpr &pos_expr = motion_pass.expr(child_index);
    visitor(PosExprWrapper(pos_expr,callbacks));
  }

  virtual std::string label() const
  {
    return "Motion Pass";
  }

  virtual int nChildren() const
  {
    return motion_pass.nExprs();
  }
};
}


std::vector<std::string> CharmapperWrapper::operationNames() const
{
  return { "Add Motion Pass" };
}


void
  CharmapperWrapper::executeOperation(
    int operation_index,
    const TreePath &path,
    OperationHandler &handler
  ) const
{
  switch (operation_index) {
    case 0:
      {
        int index = charmapper.nPasses();
        charmapper.addMotionPass();
        handler.addItem(join(path,index));
      }
      return;
  }

  assert(false);
}


void
  CharmapperWrapper::withChildWrapper(
    int child_index,const WrapperVisitor &visitor
  ) const
{
  visitor(
    MotionPassWrapper{charmapper.pass(child_index),callbacks}
  );
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
      using BodyWrapper = MotionPassWrapper::BodyWrapper;

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
