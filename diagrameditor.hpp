#ifndef DIAGRAMEDITOR_HPP_
#define DIAGRAMEDITOR_HPP_

#include <string>
#include <vector>
#include <cassert>
#include "point2d.hpp"
#include "stringutil.hpp"
#include "linetext.hpp"
#include "textobject.hpp"
#include "node2.hpp"
#include "node2texteditor.hpp"
#include "diagram.hpp"



inline Point2D operator-(const Point2D &a,const Point2D &b)
{
  float x = a.x - b.x;
  float y = a.y - b.y;
  return Point2D{x,y};
}


inline Point2D operator+(const Point2D &a,const Point2D &b)
{
  float x = a.x + b.x;
  float y = a.y + b.y;
  return Point2D{x,y};
}


struct Rect {
  Point2D start, end;

  bool contains(const Point2D &p)
  {
    return
      p.x >= start.x && p.x <= end.x &&
      p.y >= start.y && p.y <= end.y;
  }
};


inline Rect withMargin(const Rect &rect,float margin)
{
  auto offset = Point2D{margin,margin};
  return Rect{rect.start-offset,rect.end+offset};
}


struct NodeInputIndex {
  int node_index;
  int input_index;

  bool operator==(const NodeInputIndex &arg) const
  {
    return node_index==arg.node_index && input_index==arg.input_index;
  }

  bool operator!=(const NodeInputIndex &arg) const
  {
    return !operator==(arg);
  }

  static NodeInputIndex null()
  {
    return NodeInputIndex{-1,0};
  }

  void clear()
  {
    *this = null();
  }

  bool isNull() const
  {
    return *this==null();
  }
};


struct NodeConnectorIndex {
  int node_index = -1;
  int input_index = -1;
  int output_index = -1;

  static NodeConnectorIndex null()
  {
    return NodeConnectorIndex{};
  }

  bool isNull() const
  {
    return *this==null();
  }

  void clear()
  {
    *this = null();
  }

  bool operator==(const NodeConnectorIndex &arg) const
  {
    return
      node_index==arg.node_index &&
      input_index==arg.input_index &&
      output_index==arg.output_index;
  }

  bool operator!=(const NodeConnectorIndex &arg) const
  {
    return !operator==(arg);
  }
};


struct Node1Editor {
  int selected_node_index = -1;
  int focused_node_index = -1;
  bool node_was_selected = false;
  NodeInputIndex selected_node_input_index = NodeInputIndex::null();
};


struct Node2Editor {
  int selected_node_index = -1;
  int focused_node_index = -1;
  bool node_was_selected = false;
  Node2TextEditor text_editor;

  bool aNodeIsFocused() const
  {
    return focused_node_index>=0;
  }

  void unfocus()
  {
    text_editor.endEditing();
    focused_node_index = -1;
  }

  Node2& focusedNode(std::vector<Node2> &node2s)
  {
    assert(focused_node_index>=0);
    return node2s[focused_node_index];
  }

  void focusNode(int node_index,std::vector<Node2> &node2s)
  {
    focused_node_index = node_index;
    text_editor.beginEditing(focusedNode(node2s));
  }

  void selectNode(int node_index)
  {
    unfocus();
    selected_node_index = node_index;
  }

  std::string &focusedText(std::vector<Node2> &node2s)
  {
    assert(focused_node_index>=0);
    Node2 &node = focusedNode(node2s);
    return text_editor.focusedText(node);
  }
};


class DiagramEditor {
  public:
    DiagramEditor(Diagram &diagram_arg)
    : diagram(diagram_arg)
    {
    }

    int nNode2s() { return node2s().size(); }
  protected:
    Point2D mouse_press_position;
    Point2D original_node_position;
    NodeConnectorIndex selected_node2_connector_index =
      NodeConnectorIndex::null();
    Point2D temp_source_pos;
    std::vector<Node2> &node2s() { return diagram._node2s; }
    const std::vector<Node2> &node2s() const { return diagram._node2s; }
    Diagram &diagram;
    Node2Editor node2_editor;

    virtual void redraw() = 0;
    void deleteNode2(int index);
    std::string &focusedText();
    void enterPressed();
    void backspacePressed();
    void updateNodeInputs(int node_index);
    int addNode2(const std::string &text,const Point2D &position);

    void
      connectNodes(
        int input_node_index,
        int input_index,
        int output_node_index,
        int output_index
      );
};

#endif /* DIAGRAMEDITOR_HPP_ */
