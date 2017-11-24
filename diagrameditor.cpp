#include "diagrameditor.hpp"

#include <cassert>

using std::string;
using std::vector;


void DiagramEditor::deleteNode2(int index)
{
  node2s().erase(node2s().begin()+index);
}


string &DiagramEditor::focusedText()
{
  return node2_editor.focusedText(node2s());
}


void DiagramEditor::enterPressed()
{
  if (node2_editor.focused_node_index>=0) {
    node2_editor.text_editor.enter();
    redraw();
  }
}


void DiagramEditor::backspacePressed()
{
  if (node2_editor.selected_node_index>=0) {
    deleteNode2(node2_editor.selected_node_index);
    node2_editor.selected_node_index = -1;
    redraw();
    return;
  }

  if (node2_editor.aNodeIsFocused()) {
    node2_editor.text_editor.backspace();
    redraw();
    return;
  }
}


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


void DiagramEditor::textTyped(const string &new_text)
{
  if (node2_editor.aNodeIsFocused()) {
    node2_editor.text_editor.textTyped(new_text);
    redraw();
    return;
  }
}


void DiagramEditor::unfocus()
{
  node2_editor.unfocus();
  diagram.removeInvalidInputs();
}
