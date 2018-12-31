#include "nodetexteditor.hpp"


void NodeTextEditor::deletePressed()
{
  Node &node = this->node();
  std::string &focused_text = focusedText(node);
  int &cursor_column_index = cursor_position.column_index;
  int &cursor_line_index = cursor_position.line_index;
  int last_column_index = focused_text.length();

  if (cursor_column_index > last_column_index) {
    // The cursor_column_index can be beyond the end of the line so that
    // if you are using the up/down arrows to move across lines, you don't
    // lose the column you are on.  Once we try to modify a line, we
    // adjust this virtual cursor position into a real one.
    cursor_column_index = last_column_index;
  }

  int n_lines = node.lines.size();

  if (cursor_column_index == last_column_index) {
    if (cursor_line_index+1 == n_lines) {
      return;
    }

    node.joinLines(cursor_position);
    return;
  }

  node.deleteCharacter(cursor_position);
}


void NodeTextEditor::backspace(Node &node)
{
  std::string &focused_text = focusedText(node);
  int &cursor_column_index = cursor_position.column_index;
  int &cursor_line_index = cursor_position.line_index;
  int last_column_index = focused_text.length();

  if (cursor_column_index > last_column_index) {
    // The cursor_column_index can be beyond the end of the line so that
    // if you are using the up/down arrows to move across lines, you don't
    // lose the column you are on.  Once we try to modify a line, we
    // adjust this virtual cursor position into a real one.
    cursor_column_index = last_column_index;
  }

  if (cursor_column_index==0) {
    if (cursor_line_index==0) {
      return;
    }

    --cursor_line_index;
    cursor_column_index = node.lines[cursor_line_index].text.length();
    node.joinLines(cursor_position);
    return;
  }

  --cursor_column_index;
  node.deleteCharacter(cursor_position);
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

  for (const char c : new_text) {
    node.insertCharacter(cursor_position,c);
    cursor_column_index += 1;
  }
}


void NodeTextEditor::enter(Node &node,Callbacks *callbacks_ptr)
{
  int &cursor_column_index = cursor_position.column_index;
  int &cursor_line_index = cursor_position.line_index;

  node.breakLine(cursor_position);

  if (callbacks_ptr) {
    callbacks_ptr->lineUnfocused(cursor_line_index);
  }

  ++cursor_line_index;
  cursor_column_index = 0;

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


void NodeTextEditor::endEditing()
{
  node().updateInputsAndOutputs();
  node_ptr = 0;
}
