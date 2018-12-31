#include <string>
#include <cassert>
#include <iostream>
#include "diagramnode.hpp"


class NodeTextEditor {
  public:
    using Node = DiagramNode;

    struct Callbacks {
      virtual void lineUnfocused(int) = 0;
      virtual void lineFocused(int) = 0;
    };

    using CursorPosition = Node::TextPosition;

    Node *node_ptr = 0;

    NodeTextEditor()
    {
    }

    std::string &focusedText(Node &node);

    void beginEditing(Node &node);
    void endEditing();

    int cursorLineIndex() const { return cursor_position.line_index; }
    int cursorColumnIndex() const { return cursor_position.column_index; }

    CursorPosition cursorPosition() const { return cursor_position; }

    void moveCursorToLine(int new_line_index)
    {
      cursor_position.line_index = new_line_index;
    }

    void moveCursorToColumn(int new_column_index)
    {
      cursor_position.column_index = new_column_index;
    }

    void moveCursor(int new_line_index,int new_column_index)
    {
      cursor_position.line_index = new_line_index;
      cursor_position.column_index = new_column_index;
    }

    void moveCursorTo(const CursorPosition &arg)
    {
      cursor_position = arg;
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
    void deletePressed();
    void enter() { enter(node(),/*callbacks_ptr*/0); }
    void textTyped(const std::string &new_text) { textTyped(node(),new_text); }

  private:
    void backspace(Node &node);
    void left(Node& node);
    void right(Node& node);
    void up(Node &,Callbacks * callbacks_ptr);
    void down(Node &focused_node);
    void textTyped(Node &node,const std::string &new_text);
    void enter(Node &node,Callbacks *callbacks_ptr);

  private:
    CursorPosition cursor_position{0,0};
};
