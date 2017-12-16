#ifndef DIAGRAMEDITOR_HPP_
#define DIAGRAMEDITOR_HPP_

#include <string>
#include <vector>
#include <cassert>
#include "point2d.hpp"
#include "stringutil.hpp"
#include "linetext.hpp"
#include "textobject.hpp"
#include "diagramnode.hpp"
#include "nodetexteditor.hpp"
#include "diagram.hpp"
#include "circle.hpp"


struct Rect {
  Point2D start, end;

  bool contains(const Point2D &p)
  {
    return
      p.x >= start.x && p.x <= end.x &&
      p.y >= start.y && p.y <= end.y;
  }
};


struct NodeRenderInfo {
  Rect header_rect;
  Rect body_outer_rect;
  std::vector<TextObject> text_objects;
  std::vector<Circle> input_connector_circles;
  std::vector<Circle> output_connector_circles;
};



using Node = DiagramNode;


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
    DiagramEditor(Diagram &diagram_arg);
    void setDiagramPtr(Diagram *);

  protected:
    static constexpr float connector_radius = 5;
    Point2D mouse_press_position;
    Point2D original_node_position;
    NodeConnectorIndex selected_node_connector_index =
      NodeConnectorIndex::null();
    Point2D temp_source_pos;
    Diagram *diagram_ptr;
    NodeEditor node_editor;
    Diagram &diagram() const { assert(diagram_ptr); return *diagram_ptr; }

    virtual void redraw() = 0;
    virtual Rect rectAroundText(const TextObject &text_object) const = 0;
    void deleteNode(int index);
    std::string &focusedText();
    void enterPressed();
    bool aNodeIsFocused() const;
    void backspacePressed();
    void textTyped(const std::string &new_text);
    void updateNodeInputs(int node_index);
    int addNode(const std::string &text,const Point2D &position);
    void unfocus();
    Node &node(NodeIndex arg) { return diagram().node(arg); }
    const Node &node(NodeIndex arg) const { return diagram().node(arg); }
    Circle nodeOutputCircle(const Node &node,int output_index);
    bool nodeOutputContains(int node_index,int output_index,const Point2D &p);
    Circle nodeInputCircle(const Node &,int input_index);
    bool nodeInputContains(int node_index,int input_index,const Point2D &p);
    NodeConnectorIndex indexOfNodeConnectorContaining(const Point2D &p);
    TextObject
      inputTextObject(const std::string &s,float left_x,float y) const;
    Point2D
      alignmentPoint(
        const Rect &rect,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    TextObject
      alignedTextObject(
        const std::string &text,
        const Point2D &position,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    Rect nodeBodyRect(const Node &,const Rect &header_rect) const;
    Rect nodeRect(const TextObject &text_object) const;
    Rect nodeHeaderRect(const TextObject &text_object) const;
    int indexOfNodeContaining(const Point2D &p);
    NodeRenderInfo nodeRenderInfo(const Node &node) const;
    void mouseReleasedAt(Point2D mouse_release_position);

    void
      connectNodes(
        int output_node_index,
        int output_index,
        int input_node_index,
        int input_index
      );
};

#endif /* DIAGRAMEDITOR_HPP_ */
