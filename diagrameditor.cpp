#include "diagrameditor.hpp"

#include <cassert>

using std::string;


int DiagramEditor::addNode(const TextObject &text_object)
{
  int node_index = nodes.size();
  Node node;
  node.text_object = text_object;
  nodes.push_back(node);
  return node_index;
}


void DiagramEditor::deleteNode(int index)
{
  nodes.erase(nodes.begin()+index);
}


string &DiagramEditor::focusedText()
{
  assert(focused_node_index>=0);
  return nodes[focused_node_index].text_object.text;
}


void DiagramEditor::enterPressed()
{
  if (focused_node_index>=0) {
    int node_index = focused_node_index;
    selected_node_index = node_index;
    focused_node_index = -1;
    updateNodeInputs(node_index);
    redraw();
  }
}


void DiagramEditor::updateNodeInputs(int node_index)
{
  const string &text = nodes[node_index].text_object.text;
  if (text=="+") {
    nodes[node_index].inputs.resize(2);
  }
  else if (text=="ikPos") {
    nodes[node_index].inputs.resize(3);
    nodes[node_index].inputs[0].name = "global";
  }
}
