#include <string>
#include <cassert>
#include "node.hpp"


struct NodeTextEditor {
  struct Callbacks {
    virtual void lineUnfocused(int) = 0;
    virtual void lineFocused(int) = 0;
  };

  Node *node_ptr = 0;

  NodeTextEditor()
  {
  }

  std::string &focusedText(Node &node)
  {
    return node.lines[cursor_line_index].text;
  }

  void beginEditing(Node &node)
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

  int cursorLineIndex() const
  {
    return cursor_line_index;
  }

  int cursorColumnIndex() const
  {
    return cursor_column_index;
  }

  void moveCursorToLine(int new_line_index)
  {
    cursor_line_index = new_line_index;
  }

  void moveCursorToColumn(int new_column_index)
  {
    cursor_column_index = new_column_index;
  }

  void moveCursor(int new_line_index,int new_column_index)
  {
    cursor_line_index = new_line_index;
    cursor_column_index = new_column_index;
  }

  void endEditing()
  {
    node().updateInputsAndOutputs();
    node_ptr = 0;
  }

  Node& node()
  {
    assert(node_ptr);
    return *node_ptr;
  }

  void backspace() { backspace(node()); }
  void up(Callbacks &callbacks) { up(node(),&callbacks); }
  void up() { up(node(),/*callbacks_ptr*/0); }
  void down() { down(node()); }
  void left() { left(node()); }
  void right() { right(node()); }
  void enter(Callbacks &callbacks) { enter(node(),&callbacks); }
  void enter() { enter(node(),/*callbacks_ptr*/0); }
  void textTyped(const std::string &new_text) { textTyped(node(),new_text); }

  private:
    void backspace(Node &node)
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
    }

    void left(Node& node)
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

    void right(Node& node)
    {
      int last_column = node.lines[cursor_line_index].text.size();

      if (cursor_column_index==last_column) {
        return;
      }

      ++cursor_column_index;
    }

    void up(Node &,Callbacks * callbacks_ptr)
    {
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

    void down(Node &focused_node)
    {
      int n_lines = focused_node.lines.size();
      if (cursor_line_index+1 < n_lines) {
        ++cursor_line_index;
      }
    }

    void textTyped(Node &node,const std::string &new_text)
    {
      std::string &text = focusedText(node);
      int last_column = text.length();
      if (cursor_column_index>last_column) {
        cursor_column_index = last_column;
      }
      text.insert(cursor_column_index,new_text);
      cursor_column_index += new_text.length();
      node.addInputsAndOutputs();
    }

    void enter(Node &node,Callbacks *callbacks_ptr)
    {
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

  private:
    int cursor_line_index = 0;
    int cursor_column_index = 0;
};
