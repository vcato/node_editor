#ifndef DIAGRAMEDITOR_HPP_
#define DIAGRAMEDITOR_HPP_

#include <string>
#include <vector>
#include <cassert>
#include <map>
#include "point2d.hpp"
#include "stringutil.hpp"
#include "textobject.hpp"
#include "diagramnode.hpp"
#include "nodetexteditor.hpp"
#include "diagram.hpp"
#include "circle.hpp"
#include "rect.hpp"
#include "optional.hpp"


struct EventModifiers {
  bool shift_is_pressed = false;
  bool alt_is_pressed = false;
};


struct NodeRenderInfo {
  Rect header_rect;
  Rect body_outer_rect;
  std::vector<TextObject> text_objects;
  std::vector<Circle> input_connector_circles;
  std::vector<Circle> output_connector_circles;
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


class DiagramEditor {
  public:
    DiagramEditor();
    void setDiagramPtr(Diagram *);
    Diagram *diagramPtr() const;
    std::function<void()> &diagramChangedCallback();

  protected:
    using Node = DiagramNode;

    void backspacePressed();
    void escapePressed();
    void enterPressed();
    void textTyped(const std::string &new_text);
    void exportDiagramPressed();
    void importDiagramPressed();
    void leftMousePressedAt(Point2D,EventModifiers);
    void middleMousePressedAt(Point2D p,EventModifiers modifiers);
    void mouseReleasedAt(Point2D mouse_release_position);
    void mouseMovedTo(const Point2D &);

    bool aNodeIsFocused() const;
    NodeRenderInfo nodeRenderInfo(const Node &node) const;
    int nSelectedNodes() const;
    bool nodeIsSelected(NodeIndex);
    int addNode(const std::string &text,const Point2D &position);
    void selectNode(NodeIndex);
    void alsoSelectNode(NodeIndex node_index);
    void focusNode(int node_index,Diagram &diagram);
    Diagram &diagram() const { assert(diagram_ptr); return *diagram_ptr; }
    void unfocus();
    Circle nodeInputCircle(const Node &,int input_index);
    Node &node(NodeIndex arg) { return diagram().node(arg); }
    Circle nodeOutputCircle(const Node &node,int output_index);
    Rect nodeRect(const TextObject &text_object) const;
    const Node &node(NodeIndex arg) const { return diagram().node(arg); }

    void
      connectNodes(
        int output_node_index,
        int output_index,
        int input_node_index,
        int input_index
      );

    TextObject
      alignedTextObject(
        const std::string &text,
        const Point2D &position,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    NodeTextEditor text_editor;
    NodeConnectorIndex selected_node_connector_index =
      NodeConnectorIndex::null();
    Diagram *diagram_ptr;
    NodeIndex focused_node_index = noNodeIndex();
    Point2D temp_source_pos;
    Optional<Rect> maybe_selection_rectangle;
    Vector2D view_offset{0,0};

  private:
    enum class MouseMode {
      none, translate_view
    };

    virtual void redraw() = 0;
    virtual Rect rectAroundText(const TextObject &text_object) const = 0;
    virtual std::string askForSavePath() = 0;
    virtual std::string askForOpenPath() = 0;
    virtual void showError(const std::string &message) = 0;

    static NodeIndex noNodeIndex() { return -1; }
    void deleteNode(int index);
    std::string &focusedText();
    NodeIndex selectedNodeIndex() const;
    void setSelectedNodeIndex(NodeIndex arg);
    void updateNodeInputs(int node_index);
    bool nodeOutputContains(int node_index,int output_index,const Point2D &p);
    bool nodeInputContains(int node_index,int input_index,const Point2D &p);
    NodeConnectorIndex indexOfNodeConnectorContaining(const Point2D &p);
    TextObject
      inputTextObject(const std::string &s,float left_x,float y) const;
    Rect nodeBodyRect(const Node &,const Rect &header_rect) const;
    Rect nodeHeaderRect(const TextObject &text_object) const;
    int indexOfNodeContaining(const Point2D &p);
    void clearFocus();
    void clearSelection();
    Node& focusedNode(Diagram &diagram);
    bool aNodeIsSelected() const;
    void notifyDiagramChanged();
    void selectNodesInRect(const Rect &/*rect*/);
    bool nodeIsInRect(NodeIndex node_index,const Rect &rect) const;

    Point2D
      alignmentPoint(
        const Rect &rect,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    std::vector<NodeIndex> selected_node_indices;
    bool node_was_selected = false;
    static constexpr float connector_radius = 5;
    Point2D mouse_press_position;
    std::map<NodeIndex,Point2D> original_node_positions;
    std::function<void()> diagram_changed_callback;
    MouseMode mouse_mode = MouseMode::none;
    Vector2D mouse_down_view_offset;
};

#endif /* DIAGRAMEDITOR_HPP_ */
