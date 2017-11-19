#ifndef DIAGRAMEDITOR_HPP_
#define DIAGRAMEDITOR_HPP_

#include <string>
#include <vector>
#include "point2d.hpp"
#include "stringutil.hpp"


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


struct Node1 {
  struct Input {
    int source_node_index;
    std::string name;

    Input()
    : source_node_index(-1)
    {
    }
  };

  Node1()
  {
  }

  TextObject text_object;
  std::vector<Input> inputs;
};


struct Node2 {
  TextObject header_text_object;

  struct Input {
    int source_node_index = -1;
    int source_output_index = -1;
  };

  struct Line {
    std::string text;
    bool has_input = false;
    bool has_output = false;

    Line(const char *text_arg) : text(text_arg) { }
  };

  int nInputs() const
  {
    return inputs.size();
  }

  int nOutputs() const
  {
    return n_outputs;
  }

  void setNInputs(size_t arg)
  {
    inputs.resize(arg);
  }

  void setNOutputs(size_t arg)
  {
    n_outputs = arg;
  }

  void removeLine(int line_index)
  {
    lines.erase(lines.begin() + line_index);
  }

  private:
    void updateNInputs()
    {
      setNInputs(countInputs(*this));
    }

    void updateNOutputs()
    {
      setNOutputs(countOutputs(*this));
    }

  public:
    void updateInputsAndOutputs()
    {
      for (auto &line : lines) {
        line.has_input = hasInput(line.text);
        line.has_output = hasOutput(line.text);
      }

      updateNInputs();
      updateNOutputs();
    }

  std::vector<std::string> strings() const
  {
    std::vector<std::string> result;

    for (const auto &line : lines) {
      result.push_back(line.text);
    }

    return result;
  }

  static size_t countInputs(const Node2 &node)
  {
    size_t n_inputs = 0;

    for (auto &line : node.lines) {
      if (line.has_input) {
        ++n_inputs;
      }
    }

    return n_inputs;
  }

  static size_t countOutputs(const Node2 &node)
  {
    size_t n_outputs = 0;

    for (auto &line : node.lines) {
      if (line.has_output) {
        ++n_outputs;
      }
    }

    return n_outputs;
  }

  std::vector<Input> inputs;
  int n_outputs = 0;
  std::vector<Line> lines;

  static bool hasInput(const std::string &text)
  {
    if (endsWith(text,"$")) {
      return true;
    }

    return false;
  }

  static bool hasOutput(const std::string &text)
  {
    if (startsWith(text,"$")) {
      return true;
    }

    return false;
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
  int focused_node_line_index = 0;
  bool node_was_selected = false;
};


class DiagramEditor {
  public:
    void addTestNode();
  protected:
    Point2D mouse_press_position;
    Point2D original_node_position;
    NodeConnectorIndex selected_node2_connector_index =
      NodeConnectorIndex::null();
    Point2D temp_source_pos;
    std::vector<Node1> node1s;
    std::vector<Node2> node2s;
    Node1Editor node1_editor;
    Node2Editor node2_editor;

    virtual void redraw() = 0;
    int addNode(const TextObject &text_object);
    void deleteNode(int index);
    std::string &focusedText();
    void enterPressed();
    void updateNodeInputs(int node_index);
};

#endif /* DIAGRAMEDITOR_HPP_ */
