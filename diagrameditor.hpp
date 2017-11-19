#ifndef DIAGRAMEDITOR_HPP_
#define DIAGRAMEDITOR_HPP_

#include <string>
#include <vector>


#include "point2d.hpp"


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


struct TextObject {
  std::string text;
  Point2D position;
};


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


struct Node {
  struct Input {
    int source_node_index;
    std::string name;

    Input()
    : source_node_index(-1)
    {
    }
  };

  Node()
  {
  }

  TextObject text_object;
  std::vector<Input> inputs;
};


struct Node2 {
  TextObject header_text_object;
  std::vector<std::string> inputs;
  std::vector<std::string> outputs;
};


class DiagramEditor {
  protected:
    Point2D mouse_press_position;
    Point2D original_node_position;
    bool node_was_selected = false;
    int selected_node_index = -1;
    int selected_node2_index = -1;
    int focused_node_index = -1;
    NodeInputIndex selected_node_input_index = NodeInputIndex::null();
    Point2D temp_source_pos;
    std::vector<Node> nodes;
    std::vector<Node2> node2s;

    virtual void redraw() = 0;
    int addNode(const TextObject &text_object);
    void deleteNode(int index);
    std::string &focusedText();
    void enterPressed();
    void updateNodeInputs(int node_index);
};

#endif /* DIAGRAMEDITOR_HPP_ */
