#ifndef DIAGRAMEDITOR_HPP_
#define DIAGRAMEDITOR_HPP_

#include <string>
#include <vector>
#include <cassert>
#include "point2d.hpp"
#include "stringutil.hpp"
#include "linetext.hpp"
#include "textobject.hpp"
#include "node.hpp"
#include "nodetexteditor.hpp"
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


struct NodeEditor {
  int selected_node_index = -1;
  int focused_node_index = -1;
  bool node_was_selected = false;
  NodeTextEditor text_editor;

  NodeEditor()
  : text_editor()
  {
  }

  bool aNodeIsFocused() const
  {
    return focused_node_index>=0;
  }

  void unfocus()
  {
    text_editor.endEditing();
    focused_node_index = -1;
  }

  Node& focusedNode(Diagram &diagram)
  {
    return diagram.node(focused_node_index);
  }

  void focusNode(int node_index,Diagram &diagram)
  {
    focused_node_index = node_index;
    text_editor.beginEditing(focusedNode(diagram));
  }

  void selectNode(int node_index)
  {
    if (aNodeIsFocused()) {
      unfocus();
    }
    selected_node_index = node_index;
  }

  std::string &focusedText(Diagram &diagram)
  {
    assert(focused_node_index>=0);
    Node &node = focusedNode(diagram);
    return text_editor.focusedText(node);
  }
};


class DiagramEditor {
  public:
    DiagramEditor(Diagram &diagram_arg)
    : diagram(diagram_arg)
    {
    }

  protected:
    Point2D mouse_press_position;
    Point2D original_node_position;
    NodeConnectorIndex selected_node_connector_index =
      NodeConnectorIndex::null();
    Point2D temp_source_pos;
    Diagram &diagram;
    NodeEditor node_editor;

    virtual void redraw() = 0;
    void deleteNode(int index);
    std::string &focusedText();
    void enterPressed();
    void backspacePressed();
    void textTyped(const std::string &new_text);
    void updateNodeInputs(int node_index);
    int addNode(const std::string &text,const Point2D &position);
    void unfocus();
    Node &node(NodeIndex arg) { return diagram.node(arg); }
    const Node &node(NodeIndex arg) const { return diagram.node(arg); }


    void
      connectNodes(
        int output_node_index,
        int output_index,
        int input_node_index,
        int input_index
      );
};

#endif /* DIAGRAMEDITOR_HPP_ */
