#include "diagramwrapperstate.hpp"


using std::string;
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
  const Point2D &pos = node.position();
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
