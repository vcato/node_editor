#include "diagrameditor.hpp"

#include <cassert>

using std::string;


int DiagramEditor::addNode(const TextObject &text_object)
{
  int node_index = nodes.size();
  Node node;
  node.text_object = text_object;
  if (text_object.text=="+") {
    node.inputs.resize(2);
  }
  nodes.push_back(node);
  return node_index;
}


void DiagramEditor::deleteNode(int index)
{
  nodes.erase(nodes.begin()+index);
}


string &DiagramEditor::focusedText()
{
  if (focused_node_index>=0) {
    return nodes[focused_node_index].text_object.text;
  }
  else {
    assert(false);
  }
}
