#include "diagrameditor.hpp"

#include <cassert>

using std::string;
using std::vector;


#if USE_NODE1
int DiagramEditor::addNode(const TextObject &text_object)
{
  int node_index = node1s.size();
  Node1 node;
  node.text_object = text_object;
  node1s.push_back(node);
  return node_index;
}
#endif


#if USE_NODE1
void DiagramEditor::deleteNode(int index)
{
  node1s.erase(node1s.begin()+index);
}
#endif


void DiagramEditor::deleteNode2(int index)
{
  node2s().erase(node2s().begin()+index);
}


string &DiagramEditor::focusedText()
{
#if USE_NODE1
  if (node1_editor.focused_node_index>=0) {
    return node1s[node1_editor.focused_node_index].text_object.text;
  }
#endif
  return node2_editor.focusedText(node2s());
}


void DiagramEditor::enterPressed()
{
#if USE_NODE1
  if (node1_editor.focused_node_index>=0) {
    int node_index = node1_editor.focused_node_index;
    node1_editor.selected_node_index = node_index;
    node1_editor.focused_node_index = -1;
    updateNodeInputs(node_index);
    redraw();
  }
#endif
  if (node2_editor.focused_node_index>=0) {
    node2_editor.text_editor.enter();
    redraw();
  }
}


void DiagramEditor::backspacePressed()
{
#if USE_NODE1
  if (node1_editor.selected_node_index>=0) {
    deleteNode(node1_editor.selected_node_index);
    node1_editor.selected_node_index = -1;
    redraw();
    return;
  }
#endif

  if (node2_editor.selected_node_index>=0) {
    deleteNode2(node2_editor.selected_node_index);
    node2_editor.selected_node_index = -1;
    redraw();
    return;
  }

#if USE_NODE1
  if (node1_editor.focused_node_index>=0) {
    if (!focusedText().empty()) {
      focusedText().erase(focusedText().end()-1);
      redraw();
      return;
    }
  }
#endif

  if (node2_editor.aNodeIsFocused()) {
    node2_editor.text_editor.backspace();
    diagram.removeInvalidInputs();
    redraw();
    return;
  }
}


#if USE_NODE1
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
#endif


int DiagramEditor::addNode2(const std::string &text,const Point2D &position)
{
  // The node editor keeps a pointer to a node, but Nodes may move in memory.
  assert(!node2_editor.aNodeIsFocused());

  int node_index = diagram.addNode(text);
  diagram.node(node_index).header_text_object.position = position;
  return node_index;
}


void
  DiagramEditor::connectNodes(
    int input_node_index,
    int input_index,
    int output_node_index,
    int output_index
  )
{
  diagram.connectNodes(
    input_node_index,input_index,output_node_index,output_index
  );
}
