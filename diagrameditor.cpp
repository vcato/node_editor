#include "diagrameditor.hpp"

#include <cassert>

using std::string;
using std::vector;


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
  if (node1_editor.focused_node_index>=0) {
    return node1s[node1_editor.focused_node_index].text_object.text;
  }
  else {
    assert(node2_editor.focused_node_index>=0);
    return node2s[node2_editor.focused_node_index].lines[node2_editor.focused_node_line_index].text;
  }
}


void DiagramEditor::enterPressed()
{
  if (node1_editor.focused_node_index>=0) {
    int node_index = node1_editor.focused_node_index;
    node1_editor.selected_node_index = node_index;
    node1_editor.focused_node_index = -1;
    updateNodeInputs(node_index);
    redraw();
  }
  if (node2_editor.focused_node_index>=0) {
    Node2& node = node2s[node2_editor.focused_node_index];
    node.lines.insert(
      node.lines.begin() + node2_editor.focused_node_line_index + 1,
      Node2::Line("")
    );
    ++node2_editor.focused_node_line_index;
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


void DiagramEditor::addTestNode()
{
  node2s.emplace_back();
  Node2 &node = node2s.back();
  node.lines = {"a = $","b = $","$ = a+b"};
  node.header_text_object.text = "";
  node.header_text_object.position = Point2D(100,200);
  node.updateInputsAndOutputs();
  assert(node.lines[0].has_input);
}
