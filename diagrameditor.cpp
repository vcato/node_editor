#include "diagrameditor.hpp"

#include <cassert>

using std::string;


int DiagramEditor::addNode(const TextObject &text_object)
{
  int node_index = node1s.size();
  Node1 node;
  node.text_object = text_object;
  node1s.push_back(node);
  return node_index;
}


void DiagramEditor::deleteNode(int index)
{
  node1s.erase(node1s.begin()+index);
}


string &DiagramEditor::focusedText()
{
  assert(focused_node_index>=0);
  return node1s[focused_node_index].text_object.text;
}


void DiagramEditor::enterPressed()
{
  if (focused_node_index>=0) {
    int node_index = focused_node_index;
    selected_node1_index = node_index;
    focused_node_index = -1;
    updateNodeInputs(node_index);
    redraw();
  }
}


void DiagramEditor::updateNodeInputs(int node_index)
{
  const string &text = node1s[node_index].text_object.text;
  if (text=="+") {
    node1s[node_index].inputs.resize(2);
  }
  else if (text=="ikPos") {
    node1s[node_index].inputs.resize(3);
    node1s[node_index].inputs[0].name = "global";
  }
}
