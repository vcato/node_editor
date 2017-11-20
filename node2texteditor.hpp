#include <string>
#include <cassert>
#include "node2.hpp"


struct Node2TextEditor {
  int cursor_line_index = 0;
  int cursor_column_index = 0;
  Node2 *node_ptr = 0;

  std::string &focusedText(Node2 &node)
  {
    return node.lines[cursor_line_index].text;
  }

  void beginEditing(Node2 &node)
  {
    cursor_line_index = 0;
    if (node.lines.empty()) {
      cursor_column_index = 0;
    }
    else {
      cursor_column_index = node.lines[0].text.length();
    }
    node_ptr = &node;
  }

  void endEditing()
  {
    node_ptr = 0;
  }

  Node2& node()
  {
    assert(node_ptr);
    return *node_ptr;
  }

  void backspace() { backspace(node()); }
  void up() { up(node()); }
  void down() { down(node()); }
  void left() { left(node()); }
  void right() { right(node()); }
  void enter() { enter(node()); }
  void textTyped(const std::string &new_text) { textTyped(node(),new_text); }

  private:
    void backspace(Node2 &node)
    {
      std::string &focused_text = focusedText(node);
      int last_column_index = focused_text.length();

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
      node.updateInputsAndOutputs();
    }

    void left(Node2& node)
    {
      int last_column_index = focusedText(node).length();

      if (cursor_column_index>last_column_index) {
        cursor_column_index = last_column_index;
      }

      if (cursor_column_index==0) {
        return;
      }

      --cursor_column_index;
    }

    void right(Node2& node)
    {
      int last_column = node.lines[cursor_line_index].text.size();

      if (cursor_column_index==last_column) {
        return;
      }

      ++cursor_column_index;
    }

    void up(Node2 &)
    {
      if (cursor_line_index>0) {
        --cursor_line_index;
      }
    }

    void down(Node2 &focused_node)
    {
      int n_lines = focused_node.lines.size();
      if (cursor_line_index+1 < n_lines) {
        ++cursor_line_index;
      }
    }

    void textTyped(Node2 &node,const std::string &new_text)
    {
      std::string &text = focusedText(node);
      int last_column = text.length();
      if (cursor_column_index>last_column) {
        cursor_column_index = last_column;
      }
      text.insert(cursor_column_index,new_text);
      cursor_column_index += new_text.length();
      node.updateInputsAndOutputs();
    }

    void enter(Node2 &node)
    {
      {
        std::string &text = focusedText(node);
        node.lines.insert(
          node.lines.begin() + cursor_line_index + 1,
          Node2::Line(text.substr(cursor_column_index))
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
      ++cursor_line_index;
    }
};
