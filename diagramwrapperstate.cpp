#include "diagramwrapperstate.hpp"


using std::string;
using std::istream;
using std::istringstream;
using std::cerr;
using Node = DiagramNode;



static WrapperState &createChild(WrapperState &parent,const string &name)
{
  parent.children.push_back(WrapperState(name));
  return parent.children.back();
}


static WrapperState &
  createChild(WrapperState &parent,const string &name,int value)
{
  WrapperState &child = createChild(parent,name);
  child.value = value;
  return child;
}


static WrapperState &
  createChild(WrapperState &parent,const string &name,const string &value)
{
  WrapperState &child = createChild(parent,name);
  child.value = value;
  return child;
}


static WrapperState makeDiagramNodeWrapperState(const Node &node,int i)
{
  WrapperState node_state("node");
  const DiagramPoint &pos = node.position();
  int id = i+1;

  createChild(node_state,"id",id);
  {
    WrapperState &position_state = createChild(node_state,"position");
    createChild(position_state,"x",pos.x);
    createChild(position_state,"y",pos.y);
  }
  {
    int line_index = 0;
    for (auto &line : node.lines) {
      createChild(node_state,"line",line.text);
      ++line_index;
    }
  }

  int n_inputs = node.inputs.size();

  for (int input_index=0; input_index!=n_inputs; ++input_index) {
    const auto &input = node.inputs[input_index];
    NodeIndex source_node_index = input.source_node_index;
    int source_output_index = input.source_output_index;

    if (source_node_index!=nullNodeIndex()) {
      WrapperState &connection_state = createChild(node_state,"connection");
      createChild(connection_state,"input_index",input_index);
      createChild(connection_state,"source_node_id",source_node_index + 1);
      createChild(connection_state,"source_output_index",source_output_index);
    }
  }

  return node_state;
}


WrapperState makeDiagramWrapperState(const Diagram &diagram)
{
  WrapperState result("diagram");

  for (auto i : diagram.existingNodeIndices()) {
    const Node &node = diagram.node(i);
    result.children.push_back(makeDiagramNodeWrapperState(node,i));
  }

  return result;
}


static bool
  check(
    Optional<int> &maybe_x,
    const WrapperState &child_state,
    const string &expected_tag
  )
{
  if (child_state.tag==expected_tag) {
    if (child_state.value.isNumeric()) {
      maybe_x = child_state.value.asNumeric();
      return true;
    }
  }

  return false;
}


static void addNodeConnection(Node &node,const WrapperState &state)
{
  Optional<int> maybe_input_index;
  Optional<int> maybe_source_node_id;
  Optional<int> maybe_source_output_index;

  for (const auto &child_state : state.children) {
    check(maybe_input_index,child_state,"input_index") ||
    check(maybe_source_node_id,child_state,"source_node_id") ||
    check(maybe_source_output_index,child_state,"source_output_index");
  }

  if (!maybe_input_index) {
    assert(false);
  }

  int input_index = *maybe_input_index;
  int n_inputs = node.inputs.size();

  if (input_index>=n_inputs) {
    node.inputs.resize(n_inputs+1);
  }

  Node::Input &input = node.inputs[input_index];

  if (!maybe_source_node_id) {
    assert(false);
  }

  int source_node_id = *maybe_source_node_id;

  if (source_node_id<1) {
    assert(false);
  }

  input.source_node_index = source_node_id-1;

  if (!maybe_source_output_index) {
    assert(false);
  }

  int source_output_index = *maybe_source_output_index;

  if (source_output_index<0) {
    assert(false);
  }

  input.source_output_index = source_output_index;
}



static Optional<Point2D>
  maybeMakePoint2D(const WrapperState &state,string &error)
{
  Optional<int> maybe_x;
  Optional<int> maybe_y;

  for (const WrapperState &child_state : state.children) {
    check(maybe_x,child_state,"x") ||
      check(maybe_y,child_state,"y");
  }

  if (!maybe_x) {
    error = "Missing x";
    return {};
  }

  if (!maybe_y) {
    error = "Missing y";
    return {};
  }

  return Point2D(*maybe_x,*maybe_y);
}


static void addNodeTo(Diagram &diagram,const WrapperState &state,string &error)
{
  Optional<int> maybe_id;

  for (const WrapperState &child_state : state.children) {
    check(maybe_id,child_state,"id");
  }

  if (!maybe_id) {
    assert(false);
  }

  int id = *maybe_id;
  Node &node = diagram.createNode(id-1);

  for (const WrapperState &child_state : state.children) {
    if (child_state.tag=="line") {
      if (child_state.value.isString()) {
        node.lines.push_back(Node::Line(child_state.value.asString()));
      }
    }
    else if (child_state.tag=="connection") {
      addNodeConnection(node,child_state);
    }
    else if (child_state.tag=="position") {
      Optional<Point2D> maybe_position =
        maybeMakePoint2D(child_state,error);

      if (maybe_position) {
        node.setPosition(DiagramPoint(*maybe_position));
      }
      else {
        printStateOn(cerr,child_state);
        cerr << "error: " << error << "\n";
        assert(false);
      }
    }
  }
}


bool
  buildDiagramFromState(
    Diagram &diagram,
    const WrapperState &state,
    string &error
  )
{
  if (state.tag!="diagram") {
    error = "Expected tag 'diagram'";
    return false;
  }

  for (const WrapperState &child_state : state.children) {
    if (child_state.tag=="node") {
      addNodeTo(diagram,child_state,error);
    }
  }

  for (auto i : diagram.existingNodeIndices()) {
    diagram.node(i).updateInputsAndOutputs();
  }

  return true;
}


Diagram makeDiagramFromWrapperState(const WrapperState &state)
{
  Diagram diagram;
  string error;

  buildDiagramFromState(diagram,state,error);
  assert(error=="");
  return diagram;
}
