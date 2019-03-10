#ifndef DIAGRAMEDITOR_HPP_
#define DIAGRAMEDITOR_HPP_

#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <functional>
#include "point2d.hpp"
#include "stringutil.hpp"
#include "textobject.hpp"
#include "diagramnode.hpp"
#include "nodetexteditor.hpp"
#include "diagram.hpp"
#include "circle.hpp"
#include "rect.hpp"
#include "optional.hpp"
#include "viewportline.hpp"
#include "diagramstate.hpp"
#include "observeddiagram.hpp"

using DiagramRect = TaggedRect<DiagramCoordsTag>;
using ViewportRect = TaggedRect<ViewportCoordsTag>;


struct EventModifiers {
  bool shift_is_pressed = false;
  bool ctrl_is_pressed = false;
  bool alt_is_pressed = false;
};


struct NodeRenderInfo {
  ViewportRect header_rect;
  ViewportRect body_outer_rect;
  std::vector<ViewportTextObject> text_objects;
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

    void setDiagramObserver(DiagramObserverPtr);
    Diagram *diagramPtr() const;

  protected:
    using Node = DiagramNode;

    struct NodeLineIndex {
      NodeIndex node_index;
      int line_index;
    };

    void backspacePressed();
    void deletePressed();
    void escapePressed();
    void enterPressed();
    void textTyped(const std::string &new_text);
    void exportDiagramPressed();
    void importDiagramPressed();
    void leftMousePressedAt(ViewportCoords,EventModifiers);
    void middleMousePressedAt(ViewportCoords p,EventModifiers modifiers);
    void mouseReleasedAt(ViewportCoords mouse_release_position);
    void mouseMovedTo(const ViewportCoords &);

    bool aNodeIsFocused() const;
    NodeRenderInfo nodeRenderInfo(const Node &node) const;
    int nSelectedNodes() const;
    bool nodeIsSelected(NodeIndex);
    int addNode(const std::string &text,const DiagramCoords &position);
    void selectNode(NodeIndex);
    void alsoSelectNode(NodeIndex node_index);
    void focusNode(int node_index,Diagram &diagram);
    Diagram &diagram() const { assert(diagramPtr()); return *diagramPtr(); }
    void unfocus();
    Circle nodeInputCircle(const Node &,int input_index);
    Node &node(NodeIndex arg) { return diagram().node(arg); }
    Circle nodeOutputCircle(const Node &node,int output_index);
    ViewportTextObject
      viewportTextObject(
        const DiagramTextObject &diagram_text_object
      ) const;
    ViewportRect nodeRect(const DiagramTextObject &text_object) const;
    const Node &node(NodeIndex arg) const { return diagram().node(arg); }

    ViewportCoords
      viewportCoordsFromDiagramCoords(const Point2D &diagram_coords) const;

    DiagramCoords
      diagramCoordsFromViewportCoords(
        const ViewportCoords &canvas_coords
      ) const;

    void
      connectNodes(
        int output_node_index,
        int output_index,
        int input_node_index,
        int input_index
      );

    ViewportTextObject
      alignedTextObject(
        const std::string &text,
        const ViewportCoords &position,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    ViewportLine
      cursorLine(
        const Node &node,
        const NodeTextEditor::CursorPosition
      );

    ViewportLine
      cursorLine(
        NodeIndex focused_node_index,
        const NodeTextEditor::CursorPosition
      );

    std::string lineError(NodeIndex node_index,int line_index) const;
    Optional<NodeLineIndex> maybeNodeLineAt(const ViewportCoords &p) const;
    Optional<std::string> maybeToolTipTextAt(const ViewportCoords &p) const;

    NodeTextEditor text_editor;
    NodeConnectorIndex selected_node_connector_index =
      NodeConnectorIndex::null();
    Diagram *diagram_ptr2 = nullptr;
    DiagramObserverPtr diagram_observer_ptr;
    const DiagramState *diagram_state_ptr = nullptr;
    NodeIndex focused_node_index = noNodeIndex();
    Point2D temp_source_pos;
    Optional<ViewportRect> maybe_selection_rectangle;
    Vector2D view_offset{0,0};
    void notifyDiagramChanged();

  private:
    enum class MouseMode {
      none, translate_view
    };

    virtual void redraw() = 0;
    virtual ViewportRect rectAroundText(const std::string &) const = 0;
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

    ViewportRect
      rectAroundTextObject(const ViewportTextObject &text_object) const;

    ViewportTextObject
      inputTextObject(const std::string &s,float left_x,float y) const;

    ViewportRect
      nodeBodyRect(const Node &,const ViewportRect &header_rect) const;

    ViewportRect nodeHeaderRect(const DiagramTextObject &text_object) const;
    bool
      nodeContains(
        NodeIndex node_index,
        const ViewportCoords &p
      ) const;
    NodeIndex indexOfNodeContaining(const ViewportCoords &p) const;
    void clearFocus();
    void clearSelection();
    Node& focusedNode(Diagram &diagram);
    bool aNodeIsSelected() const;
    void selectNodesInRect(const ViewportRect &);
    bool nodeIsInRect(NodeIndex node_index,const ViewportRect &rect) const;
    virtual ViewportLine
      textObjectCursorLine(
        const ViewportTextObject &text_object,
        int column_index
      ) const = 0;

    ViewportCoords
      alignmentPoint(
        const ViewportRect &rect,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    int
      closestColumn(
        const ViewportTextObject &line_text_object,
        const ViewportCoords &
      ) const;

    struct CursorPositionFinder;

    NodeTextEditor::CursorPosition
      closestCursorPositionTo(
        NodeIndex node_index,
        const ViewportCoords &p
      ) const;

    void setDiagramPtr(Diagram *);
    void setDiagramStatePtr(const DiagramState *);

    std::vector<NodeIndex> selected_node_indices;
    bool node_was_selected = false;
    static constexpr float connector_radius = 5;
    ViewportCoords mouse_press_position;
    std::map<NodeIndex,DiagramCoords> original_node_positions;
    MouseMode mouse_mode = MouseMode::none;
    Vector2D mouse_down_view_offset;
};

#endif /* DIAGRAMEDITOR_HPP_ */
