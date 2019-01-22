#include "charmapperwrapper.hpp"

#include <iostream>
#include <algorithm>
#include "streamvector.hpp"
#include "defaultdiagrams.hpp"
#include "diagramwrapperstate.hpp"
#include "namewrapper.hpp"

using std::cerr;
using std::vector;
using std::string;
using WrapperData = CharmapperWrapper::WrapperData;
using VariablePass = Charmapper::VariablePass;
using Variable = Charmapper::Variable;
using Label = CharmapperWrapper::Label;
using Channel = Charmapper::Channel;


template <typename T>
static int indexOf(const T &item,const std::vector<T> &container)
{
  auto iter = std::find(container.begin(),container.end(),item);

  assert(iter!=container.end());
  return iter-container.begin();
}


static int
  findIndex(
    const EnumerationWrapper &wrapper,
    const WrapperValue::Enumeration &value
  )
{
  const string &name = value.name;
  const auto &enumeration_names = wrapper.enumerationNames();
  int n_enumeration_names = enumeration_names.size();

  for (int i=0; i!=n_enumeration_names; ++i) {
    if (makeTag(enumeration_names[i])==name) {
      return i;
    }
  }

  cerr << "Couldn't find " << name << " in " << wrapper.label() << "\n";
  assert(false);
}


static void setChildren(const Wrapper &wrapper,const WrapperState &state)
{
  int n_children = wrapper.nChildren();

  for (const WrapperState &child_state : state.children) {
    bool found = false;

    for (int i=0; i!=n_children; ++i) {
      wrapper.withChildWrapper(i,[&](const Wrapper &child_wrapper){
        if (makeTag(child_wrapper.label())==child_state.tag) {
          child_wrapper.setState(child_state);
          found = true;
        }
      });
    }

    if (wrapper.diagramPtr()) {
      if (child_state.tag=="diagram") {
        *wrapper.diagramPtr() = makeDiagramFromWrapperState(child_state);
        found = true;
      }
    }

    if (!found) {
      cerr << "Couldn't find tag " << child_state.tag << " in " <<
        wrapper.label() << "\n";
    }
  }
}


namespace {
struct ChannelWrapper : LeafWrapper<NumericWrapper> {
  Channel &channel;
  const char *label_member;
  const WrapperData &callbacks;

  ChannelWrapper(
    Channel &channel_arg,
    const char *label_arg,
    const WrapperData &callbacks_arg
  )
  : channel(channel_arg),
    label_member(label_arg),
    callbacks(callbacks_arg)
  {
  }

  std::vector<OperationName> operationNames() const override
  {
    return {};
  }

  void
    executeOperation(
      int /*operation_index*/,
      const TreePath &/*path*/,
      TreeObserver &
    ) const override
  {
    assert(false);
  }

  Diagram *diagramPtr() const override
  {
    if (channel.optional_diagram) {
      return &*channel.optional_diagram;
    }

    // Channels will have diagrams at some point, but they don't have them
    // yet.
    return nullptr;
  }

  DiagramObserverPtr makeDiagramObserver() const override
  {
    if (!channel.optional_diagram) {
      channel.optional_diagram.emplace();
    }

    return callbacks.makeDiagramObserver(*channel.optional_diagram);
  }

  Label label() const override
  {
    return label_member;
  }

  void setValue(Value arg) const override
  {
    channel.value = arg;
    callbacks.notifyCharmapChanged();
  }

  Value value() const override
  {
    return channel.value;
  }

  void setState(const WrapperState &state) const override
  {
    if (state.value.isNumeric()) {
      setValue(state.value.asNumeric());
    }
    else {
      assert(false);
    }
  }

  bool canEditDiagram() const override
  {
    assert(false);
  }
};
}


namespace {
struct MotionPassWrapper : VoidWrapper {
  private: using Self = MotionPassWrapper;
  public:
  using MotionPass = Charmapper::MotionPass;
  using Position = Charmapper::Position;
  using PosExpr = MotionPass::PosExpr;
  using GlobalPositionData = Charmapper::GlobalPosition::Data;
  using GlobalPosition = Charmapper::GlobalPosition;
  using FromBodyGlobalPositionData = Charmapper::GlobalPosition::FromBodyData;
  using ComponentsGlobalPositionData =
    Charmapper::GlobalPosition::ComponentsData;

  Charmapper &charmapper;
  MotionPass &motion_pass;
  const int pass_index;
  const WrapperData &callbacks;

  struct PositionWrapper : NoOperationWrapper<VoidWrapper> {
    Position &position;
    const char *label_member;
    const WrapperData &callbacks;
    const Diagram *default_diagram_ptr;

    PositionWrapper(
      Position &position_arg,
      const char *label_arg,
      const WrapperData &callbacks_arg,
      const Diagram *default_diagram_ptr_arg
    )
    : position(position_arg),
      label_member(label_arg),
      callbacks(callbacks_arg),
      default_diagram_ptr(default_diagram_ptr_arg)
    {
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

    virtual Label label() const
    {
      return label_member;
    }

    void setState(const WrapperState &state) const override
    {
      setChildren(*this,state);
    }

    Diagram *diagramPtr() const override
    {
      return &position.diagram;
    }

    DiagramObserverPtr makeDiagramObserver() const override
    {
      return callbacks.makeDiagramObserver(position.diagram);
    }

    const Diagram& defaultDiagram() const override
    {
      assert(default_diagram_ptr);
      return *default_diagram_ptr;
    }
  };

  struct FromBodyGlobalPositionWrapper : NoOperationWrapper<VoidWrapper> {
    FromBodyGlobalPositionData &from_body_global_position;
    const WrapperData &callbacks;

    FromBodyGlobalPositionWrapper(
      FromBodyGlobalPositionData &arg,
      const WrapperData &callbacks_arg
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
            from_body_global_position.source_body_link,
            callbacks
          )
        );
      }
      else if (child_index==1) {
        visitor(PositionWrapper(
          from_body_global_position.local_position,
          "Local Position",
          callbacks,
          &from_body_global_position.defaultLocalPositionDiagram()
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

    virtual Label label() const
    {
      assert(false);
    }

    virtual int nChildren() const
    {
      return 2;
    }

    void setState(const WrapperState &) const override
    {
      assert(false);
    }
  };

  struct ComponentsGlobalPositionWrapper : NoOperationWrapper<VoidWrapper> {
    ComponentsGlobalPositionData &components_global_position;
    const WrapperData &callbacks;

    ComponentsGlobalPositionWrapper(
      ComponentsGlobalPositionData &arg,
      const WrapperData &callbacks_arg
    )
    : components_global_position(arg),
      callbacks(callbacks_arg)
    {
    }

    void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
    {
      switch (child_index) {
        case 0:
          visitor(ChannelWrapper(components_global_position.x,"X",callbacks));
          return;
        case 1:
          visitor(ChannelWrapper(components_global_position.y,"Y",callbacks));
          return;
      }
    }

    virtual Diagram *diagramPtr() const
    {
      assert(false);
    }

    virtual Label label() const
    {
      assert(false);
    }

    virtual int nChildren() const
    {
      return 2;
    }

    void setState(const WrapperState &) const override
    {
      assert(false);
    }
  };

  struct GlobalPositionWrapper : NoOperationWrapper<EnumerationWrapper> {
    GlobalPosition &global_position;
    const WrapperData &callbacks;

    GlobalPositionWrapper(
      GlobalPosition &global_position_arg,
      const WrapperData &callbacks_arg
    )
    : global_position(global_position_arg),
      callbacks(callbacks_arg)
    {
    }

    struct ValueVisitor : GlobalPositionData::Visitor {
      const WrapperVisitor &visitor;
      const WrapperData &callbacks;

      ValueVisitor(
        const WrapperVisitor &visitor_arg,
        const WrapperData &callbacks_arg
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
        visitor(ComponentsGlobalPositionWrapper(data,callbacks));
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

    DiagramObserverPtr makeDiagramObserver() const override
    {
      return callbacks.makeDiagramObserver(global_position.diagram);
    }

    const Diagram& defaultDiagram() const override
    {
      if (global_position.isComponents()) {
        return global_position.defaultComponentsDiagram();
      }

      if (global_position.isFromBody()) {
        return global_position.defaultFromBodyDiagram();
      }

      assert(false);
    }

    void setIndex(int index) const
    {
      switch (index) {
        case 0:
          // Components
          global_position.switchToComponents();
          break;
        case 1:
          // From Body
          global_position.switchToFromBody();
          break;
        default:
          assert(false);
      }
    }

    virtual void
      setValue(
        const TreePath &path,
        int index,
        TreeObserver &tree_observer
      ) const
    {
      setIndex(index);
      tree_observer.itemReplaced(path);
    }

    Index value() const override
    {
      if (global_position.isComponents()) {
        return 0;
      }

      if (global_position.isFromBody()) {
        return 1;
      }

      assert(false);
    }

    vector<string> enumerationNames() const override
    {
      return {"Components","From Body"};
    }

    virtual Label label() const
    {
      return "Global Position";
    }

    void setState(const WrapperState &state) const override
    {
      if (state.value.isEnumeration()) {
        setIndex(findIndex(*this,state.value.asEnumeration()));
      }
      else {
        assert(false);
      }

      setChildren(*this,state);
    }
  };

  struct BodyWrapper : NoOperationWrapper<LeafWrapper<EnumerationWrapper>> {
    const WrapperData &callbacks;
    BodyLink &body_link;
    const char *label_member;

    BodyWrapper(
      const char *label_arg,
      BodyLink &body_link_arg,
      const WrapperData &callbacks_arg
    )
    : callbacks(callbacks_arg),
      body_link(body_link_arg),
      label_member(label_arg)
    {
    }

    Label label() const override { return label_member; }

    void
      handleSceneChange(
        Wrapper::TreeObserver &tree_observer,
        const TreePath &path_of_this
      ) const;

    vector<string> enumerationNames() const override
    {
      vector<string> result;
      result.push_back("None");
      vector<string> body_names = callbacks.scene_list.allBodyNames();
      result.insert(result.end(),body_names.begin(),body_names.end());
      return result;
    }

    void setIndex(int index) const
    {
      assert(index>=0);

      if (index==0) {
        body_link.clear();
      }
      else {
        body_link = callbacks.scene_list.allBodyLinks()[index-1];
      }
    }

    void
      setValue(
        const TreePath &,
        int index,
        TreeObserver &
      ) const override
    {
      setIndex(index);
      callbacks.notifyCharmapChanged();
    }

    Index value() const override
    {
      if (!body_link.hasValue()) {
        return 0;
      }

      return indexOf(body_link,callbacks.scene_list.allBodyLinks()) + 1;
    }

    void setState(const WrapperState &state) const override
    {
      if (!state.value.isVoid()) {
        if (state.value.isEnumeration()) {
          setIndex(findIndex(*this,state.value.asEnumeration()));
        }
        else {
          assert(false);
        }
      }

      for (const WrapperState &child_state : state.children) {
        cerr << "child_state.tag: " << child_state.tag << "\n";
        assert(false);
      }
    }
  };

  struct PosExprWrapper;

  MotionPassWrapper(
    Charmapper &charmapper_arg,
    Charmapper::MotionPass &motion_pass_arg,
    int pass_index_arg,
    const WrapperData &callbacks_arg
  )
  : charmapper(charmapper_arg),
    motion_pass(motion_pass_arg),
    pass_index(pass_index_arg),
    callbacks(callbacks_arg)
  {
  }

  void
    addPosExprOperation(
      const TreePath &path,
      TreeObserver &tree_observer
    ) const
  {
    int index = motion_pass.nExprs();
    motion_pass.addPosExpr();
    tree_observer.itemAdded(join(path,index));
  }

  void
    insertVariablePassOperation(
      const TreePath &path,
      TreeObserver &tree_observer
    ) const
  {
    charmapper.insertVariablePass(pass_index);

    tree_observer.itemAdded(path);
  }

  void
    removeOperation(
      const TreePath &path,
      TreeObserver &tree_observer
    ) const
  {
    charmapper.removePass(pass_index);
    tree_observer.itemRemoved(path);
  }

  struct OperationTableEntry {
    const char *name;
    void (Self::*method)(const TreePath &,TreeObserver &) const;
  };

  using OperationTable = std::vector<OperationTableEntry>;

  static OperationTable operationTable()
  {
    return {
      {"Add Pos Expr",        &Self::addPosExprOperation},
      {"Insert Variable Pass",&Self::insertVariablePassOperation},
      {"Remove",              &Self::removeOperation}
    };
  }

  static std::vector<std::string>
    operationNamesOf(const OperationTable &table)
  {
    std::vector<std::string> names;
    names.reserve(table.size());

    for (auto &entry : table) {
      names.push_back(entry.name);
    }

    return names;
  }

  std::vector<std::string> operationNames() const override
  {
    return operationNamesOf(operationTable());
  }

  void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &tree_observer
    ) const override
  {
    auto method = operationTable()[operation_index].method;
    return (this ->* method)(path,tree_observer);
  }

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const;

  virtual Label label() const
  {
    return "Motion Pass";
  }

  virtual int nChildren() const
  {
    return motion_pass.nExprs();
  }

  void setState(const WrapperState &state) const override;
};
}


static string varName(int var_number)
{
  return "var" + std::to_string(var_number);
}


static bool
  hasVariable(const VariablePass &variable_pass,const string &var_name)
{
  for (auto &var : variable_pass.variables) {
    if (var.name == var_name) {
      return true;
    }
  }

  return false;
}


static string firstUnusedVariableName(const VariablePass &variable_pass)
{
  int var_number = 1;

  for (;;) {
    string var_name = varName(var_number);

    if (!hasVariable(variable_pass,var_name)) {
      return var_name;
    }

    ++var_number;
  }
}


namespace {
struct VariableWrapper : NoOperationWrapper<VoidWrapper> {
  Variable &variable;
  const WrapperData &wrapper_data;

  VariableWrapper(Variable &variable_arg,const WrapperData &wrapper_data_arg)
  : variable(variable_arg),
    wrapper_data(wrapper_data_arg)
  {
  }

  int nChildren() const override
  {
    return 1;
  }

  void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const override
  {
    if (child_index == 0) {
      return visitor(ChannelWrapper(variable.value,"value",wrapper_data));
    }

    assert(false);
  }

  Label label() const override
  {
    return variable.name;
  }

  bool labelCanBeChanged() const override
  {
    return true;
  }

  void setLabel(const Label &new_label) const override
  {
    variable.name = new_label;
    wrapper_data.notifyCharmapChanged();
  }

  void setState(const WrapperState &) const override
  {
    assert(false);
  }
};
}


namespace {
struct VariablePassWrapper : VoidWrapper {
  VariablePass &variable_pass;
  const WrapperData &wrapper_data;

  VariablePassWrapper(
    VariablePass &variable_pass_arg,
    const WrapperData &wrapper_data_arg
  )
  : variable_pass(variable_pass_arg),
    wrapper_data(wrapper_data_arg)
  {
  }

  int nChildren() const override
  {
    return variable_pass.variables.size();
  }

  Label label() const override { return "Variable Pass"; }

  void
    withChildWrapper(
      int child_index,
      const WrapperVisitor &visitor
    ) const override
  {
    visitor(VariableWrapper{variable_pass.variables[child_index],wrapper_data});
  }

  void setState(const WrapperState &) const override
  {
    assert(false); // not implemented
  }

  std::vector<OperationName> operationNames() const override
  {
    return {"Add Variable"};
  }

  void
    executeAddVariableOperation(
      const TreePath &variable_pass_path,
      TreeObserver &tree_observer
    ) const
  {
    VariablePass::VariableIndex new_variable_index =
      variable_pass.addVariable(firstUnusedVariableName(variable_pass));

#if 1
    TreePath new_variable_path = join(variable_pass_path,new_variable_index);
    tree_observer.itemAdded(new_variable_path);
#else
    // We'll need to notify the tree observer of the new item.
#endif
  }

  void
    executeOperation(
      int operation_index,
      const TreePath &variable_pass_path,
      TreeObserver &tree_observer
    ) const override
  {
    switch (operation_index) {
      case 0:
        executeAddVariableOperation(variable_pass_path,tree_observer);
        return;
    }

    assert(false);
  }
};
}


struct MotionPassWrapper::PosExprWrapper : VoidWrapper {
  MotionPass &motion_pass;
  int index;
  const WrapperData &callbacks;

  PosExprWrapper(
    MotionPass &motion_pass_arg,
    int index_arg,
    const WrapperData &callbacks_arg
  )
  : motion_pass(motion_pass_arg),
    index(index_arg),
    callbacks(callbacks_arg)
  {
  }

  PosExpr &posExpr() const
  {
    return motion_pass.expr(index);
  }

  void
    executeRemoveOperation(
      const TreePath &path,
      TreeObserver &tree_observer
    ) const
  {
    tree_observer.itemRemoved(path);
    motion_pass.removePosExpr(index);
  }

  virtual std::vector<OperationName> operationNames() const
  {
    return {"Remove"};
  }

  virtual void
    executeOperation(
      int operation_index,
      const TreePath &path,
      TreeObserver &tree_observer
    ) const
  {
    if (operation_index==0) {
      executeRemoveOperation(path,tree_observer);
      return;
    }

    assert(false);
  }

  static const int target_body_index = 0;
  static const int local_position_index = 1;
  static const int global_position_index = 2;

  void withChildWrapper(int child_index,const WrapperVisitor &visitor) const
  {
    if (child_index==target_body_index) {
      // Target body
      visitor(
        BodyWrapper("Target Body",posExpr().target_body_link,callbacks)
      );
    }
    else if (child_index==local_position_index) {
      visitor(
        PositionWrapper(
          posExpr().local_position,
          "Local Position",
          callbacks,
          &posExpr().defaultLocalPositionDiagram()
        )
      );
    }
    else if (child_index==global_position_index) {
      visitor(
        GlobalPositionWrapper(posExpr().global_position,callbacks)
      );
    }
    else {
      assert(false);
    }
  }

  virtual Label label() const
  {
    return "Pos Expr";
  }

  virtual int nChildren() const
  {
    return 3;
  }

  void setState(const WrapperState &state) const override
  {
    setChildren(*this,state);
  }

  Diagram *diagramPtr() const override
  {
    return &posExpr().diagram;
  }

  DiagramObserverPtr makeDiagramObserver() const override
  {
    return callbacks.makeDiagramObserver(posExpr().diagram);
  }

  const Diagram &defaultDiagram() const override
  {
    return PosExpr::defaultDiagram();
  }
};


void
  MotionPassWrapper::withChildWrapper(
    int child_index,
    const WrapperVisitor &visitor
  ) const
{
  visitor(PosExprWrapper(motion_pass,child_index,callbacks));
}


void
  MotionPassWrapper::BodyWrapper::handleSceneChange(
    Wrapper::TreeObserver &tree_observer,
    const TreePath &path_of_this
  ) const
{
  bool found = false;

  for (auto &one_body_link : callbacks.scene_list.allBodyLinks()) {
    if (one_body_link == body_link) {
      found = true;
    }
  }

  if (!found) {
    body_link.clear();
  }

  tree_observer.itemReplaced(path_of_this);

  // We notify the tree observer that the charmap changed, but we don't
  // call back saying that the charmap changed.  Whoever is calling this
  // will just have to assume that the charmap changed and reapply the
  // charmaps.
}


void MotionPassWrapper::setState(const WrapperState &state) const
{
  int child_index = 0;

  for (const WrapperState &child_state : state.children) {
    if (child_state.tag=="pos_expr") {
      motion_pass.addPosExpr();
      PosExprWrapper(motion_pass,child_index,callbacks).setState(child_state);
    }
    else {
      assert(false);
    }
    ++child_index;
  }
}


std::vector<std::string> CharmapperWrapper::operationNames() const
{
  return { "Add Motion Pass", "Remove" };
}


void CharmapperWrapper::setState(const WrapperState &state) const
{
  int n = state.children.size();

  for (int i=0; i!=n; ++i) {
    if (state.children[i].tag=="motion_pass") {
      charmapper.addMotionPass();
      withChildWrapper(
        i,
        [&](const Wrapper &child_wrapper){
          child_wrapper.setState(state.children[i]);
        }
      );
    }
  }
}


void
  CharmapperWrapper::executeOperation(
    int operation_index,
    const TreePath &path,
    TreeObserver &tree_observer
  ) const
{
  switch (operation_index) {
    case 0:
      {
        int index = charmapper.nPasses();
        charmapper.addMotionPass();
        tree_observer.itemAdded(join(path,index));
      }
      return;
    case 1:
      {
        tree_observer.itemRemoved(path);
        wrapper_data.removeCharmapper();
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
  if (auto *motion_pass_ptr = charmapper.maybeMotionPass(child_index)) {
    visitor(
      MotionPassWrapper{
        charmapper,
        *motion_pass_ptr,
        child_index,
        wrapper_data
      }
    );
  }
  else if (
    auto *variable_pass_ptr = charmapper.maybeVariablePass(child_index)
  ) {
    visitor(
      VariablePassWrapper{
        *variable_pass_ptr,
        wrapper_data
      }
    );
  }
  else {
    assert(false); // not implemented
  }
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
    Charmapper &charmapper,
    const std::string &charmapper_name,
    TreeObserver &tree_observer,
    const TreePath &charmapper_path,
    ObservedDiagrams &observed_diagrams,
    const SceneList &scene_list
  )
{
  // We just have stub callbacks here so that we can create the
  // CharmapperWrapper.  CharmapperWrapper::handleSceneChange won't
  // use the callbacks.

  struct Callbacks : CharmapperWrapper::Callbacks {
    void notifyCharmapChanged() const override
    {
      // MotionPassWrapper::BodyWrapper could potentially call this, but
      // right now it makes the assumption that the caller knows that
      // a charmap change is a possibility and doesn't actually call this.
      assert(false);
    }

    void removeCharmapper() const override
    {
      assert(false);
    }
  };

  Callbacks callbacks;

  bool diagram_observer_created = false;
  CharmapperWrapper::WrapperData wrapper_data{
    scene_list,
    observed_diagrams,
    callbacks,
    diagram_observer_created
  };
    // We might want to make observed_diagrams be optional in the
    // wrapper data, since we don't actually need the observed diagrams to
    // handle a scene change.

  CharmapperWrapper(
    charmapper,
    wrapper_data,
    charmapper_name
  ).handleSceneChange(tree_observer,charmapper_path);
}


void
  CharmapperWrapper::handleSceneChange(
    TreeObserver &tree_observer,
    const TreePath &path_of_this
  )
{
  // When the scene changes, the list of bodies that are shown as options for
  // source/target bodies needs to be updated.
  // We call tree_observer.enumerationValuesChanged() for the appropriate
  // paths.

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
          tree_observer,path_of_sub_wrapper
        );
      }
    }
  );
}


Label CharmapperWrapper::label() const
{
  return label_member;
}
