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


void DiagramEditor::deleteNode2(int index)
{
  node2s.erase(node2s.begin()+index);
}


string &DiagramEditor::focusedText()
{
  if (node1_editor.focused_node_index>=0) {
    return node1s[node1_editor.focused_node_index].text_object.text;
  }
  else {
    return node2_editor.focusedText(node2s);
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
    node2_editor.text_editor.enter();
    redraw();
  }
}


void DiagramEditor::backspacePressed()
{
  if (node1_editor.selected_node_index>=0) {
    deleteNode(node1_editor.selected_node_index);
    node1_editor.selected_node_index = -1;
    redraw();
    return;
  }

  if (node2_editor.selected_node_index>=0) {
    deleteNode2(node2_editor.selected_node_index);
    node2_editor.selected_node_index = -1;
    redraw();
    return;
  }

  if (node1_editor.focused_node_index>=0) {
    if (!focusedText().empty()) {
      focusedText().erase(focusedText().end()-1);
      redraw();
      return;
    }
  }

  if (node2_editor.aNodeIsFocused()) {
    node2_editor.text_editor.backspace();
    redraw();
    return;
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


int DiagramEditor::addNode2(const std::string &text,const Point2D &position)
{
  int node_index = node2s.size();

  node2s.emplace_back();
  Node2 &node = node2s[node_index];
  node.setText(text);
  node.header_text_object.text = "";
  node.header_text_object.position = position;

  return node_index;
}
