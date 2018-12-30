#include "nodetexteditor.hpp"


void NodeTextEditor::backspace(Node &node)
{
  std::string &focused_text = focusedText(node);
  int last_column_index = focused_text.length();
  int &cursor_column_index = cursor_position.column_index;
  int &cursor_line_index = cursor_position.line_index;

  if (cursor_column_index>last_column_index) {
    cursor_column_index = last_column_index;
  }

  if (cursor_column_index==0) {
    if (cursor_line_index==0) {
      return;
    }
    cursor_column_index =
      node.lines[cursor_line_index-1].text.length();
    node.lines[cursor_line_index-1].text +=
      node.lines[cursor_line_index].text;
    node.removeLine(cursor_line_index);
    --cursor_line_index;
    node.updateInputsAndOutputs();
    return;
  }

  focused_text.erase(focused_text.begin()+(cursor_column_index-1));
  --cursor_column_index;
}


void NodeTextEditor::left(Node& node)
{
  int last_column_index = focusedText(node).length();
  int &cursor_column_index = cursor_position.column_index;

  if (cursor_column_index>last_column_index) {
    cursor_column_index = last_column_index;
  }

  if (cursor_column_index==0) {
    return;
  }

  --cursor_column_index;
}


void NodeTextEditor::right(Node& node)
{
  int &cursor_column_index = cursor_position.column_index;
  int &cursor_line_index = cursor_position.line_index;
  int last_column = node.lines[cursor_line_index].text.size();

  if (cursor_column_index==last_column) {
    return;
  }

  ++cursor_column_index;
}


void NodeTextEditor::up(Node &,Callbacks * callbacks_ptr)
{
  int &cursor_line_index = cursor_position.line_index;

  if (cursor_line_index>0) {
    if (callbacks_ptr) {
      callbacks_ptr->lineUnfocused(cursor_line_index);
    }
    --cursor_line_index;
    if (callbacks_ptr) {
      callbacks_ptr->lineFocused(cursor_line_index);
    }
  }
}


void NodeTextEditor::down(Node &focused_node)
{
  int &cursor_line_index = cursor_position.line_index;

  int n_lines = focused_node.lines.size();

  if (cursor_line_index+1 < n_lines) {
    ++cursor_line_index;
  }
}


void NodeTextEditor::textTyped(Node &node,const std::string &new_text)
{
  int &cursor_column_index = cursor_position.column_index;

  std::string &text = focusedText(node);
  int last_column = text.length();
  if (cursor_column_index>last_column) {
    cursor_column_index = last_column;
  }
  text.insert(cursor_column_index,new_text);
  cursor_column_index += new_text.length();
  node.addInputsAndOutputs();
}


void NodeTextEditor::enter(Node &node,Callbacks *callbacks_ptr)
{
  int &cursor_column_index = cursor_position.column_index;
  int &cursor_line_index = cursor_position.line_index;

  {
    std::string &text = focusedText(node);
    node.lines.insert(
      node.lines.begin() + cursor_line_index + 1,
      Node::Line(text.substr(cursor_column_index))
    );
  }
  {
    std::string &text = node.lines[cursor_line_index].text;
    node.lines[cursor_line_index].text.erase(
      text.begin() + cursor_column_index,
      text.end()
    );
  }
  node.updateInputsAndOutputs();
  cursor_column_index = 0;
  if (callbacks_ptr) {
    callbacks_ptr->lineUnfocused(cursor_line_index);
  }
  ++cursor_line_index;
  if (callbacks_ptr) {
    callbacks_ptr->lineFocused(cursor_line_index);
  }
}


std::string &NodeTextEditor::focusedText(Node &node)
{
  int &cursor_line_index = cursor_position.line_index;

  return node.lines[cursor_line_index].text;
}


void NodeTextEditor::beginEditing(Node &node)
{
  int &cursor_column_index = cursor_position.column_index;
  int &cursor_line_index = cursor_position.line_index;

  cursor_line_index = 0;
  if (node.lines.empty()) {
    cursor_column_index = 0;
  }
  else {
    cursor_column_index = node.lines[0].text.length();
  }
  node_ptr = &node;
}
