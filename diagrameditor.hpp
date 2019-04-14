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
#include "viewportcircle.hpp"
#include "rect.hpp"
#include "optional.hpp"
#include "viewportline.hpp"
#include "diagramevaluationstate.hpp"
#include "observeddiagram.hpp"
#include "viewportrect.hpp"

using DiagramRect = TaggedRect<DiagramCoordsTag>;


struct EventModifiers {
  bool shift_is_pressed = false;
  bool ctrl_is_pressed = false;
  bool alt_is_pressed = false;
};


struct NodeRenderInfo {
  ViewportRect header_rect;
  ViewportRect body_outer_rect;
  std::vector<ViewportTextObject> text_objects;
  std::vector<ViewportCircle> input_connector_circles;
  std::vector<ViewportCircle> output_connector_circles;
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
    void leftMousePressedAt(ViewportPoint,EventModifiers);
    void middleMousePressedAt(ViewportPoint,EventModifiers modifiers);
    void mouseReleasedAt(ViewportPoint mouse_release_position);
    void mouseMovedTo(const ViewportPoint &);

    bool aNodeIsFocused() const;
    NodeRenderInfo nodeRenderInfo(const Node &node) const;
    NodeRenderInfo nodeRenderInfo2(const Node &node) const;
    int nSelectedNodes() const;
    bool nodeIsSelected(NodeIndex);
    int addNode(const std::string &text,const DiagramPoint &position);
    void selectNode(NodeIndex);
    void alsoSelectNode(NodeIndex node_index);
    void focusNode(int node_index,Diagram &diagram);
    Diagram &diagram() const { assert(diagramPtr()); return *diagramPtr(); }
    void unfocus();
    ViewportCircle nodeInputCircle(const Node &,int input_index);
    Node &node(NodeIndex arg) { return diagram().node(arg); }
    ViewportCircle nodeOutputCircle(const Node &node,int output_index);
    ViewportTextObject
      viewportTextObject(
        const DiagramTextObject &diagram_text_object
      ) const;
    const Node &node(NodeIndex arg) const { return diagram().node(arg); }

    DiagramVector
      diagramVectorFromViewportVector(const ViewportVector &) const;

    ViewportPoint viewportCoordsFromDiagramCoords(const DiagramPoint &) const;

    DiagramPoint
      diagramCoordsFromViewportCoords(
        const ViewportPoint &canvas_coords
      ) const;

    void
      connectNodes(
        int output_node_index,
        int output_index,
        int input_node_index,
        int input_index
      );

    ViewportLine
      cursorLine(
        const Node &node,
        const NodeTextEditor::CursorPosition
      ) const;

    ViewportLine
      cursorLine(
        NodeIndex focused_node_index,
        const NodeTextEditor::CursorPosition
      ) const;

    std::string lineError(NodeIndex node_index,int line_index) const;
    Optional<NodeLineIndex> maybeNodeLineAt(const ViewportPoint &p) const;
    Optional<std::string> maybeToolTipTextAt(const ViewportPoint &p) const;

    NodeTextEditor text_editor;
    NodeConnectorIndex selected_node_connector_index =
      NodeConnectorIndex::null();
    Diagram *diagram_ptr2 = nullptr;
    DiagramObserverPtr diagram_observer_ptr;
    const DiagramEvaluationState *diagram_state_ptr = nullptr;
    NodeIndex focused_node_index = noNodeIndex();
    ViewportPoint temp_source_pos;
    Optional<ViewportRect> maybe_selection_rectangle;
    ViewportVector view_offset{0,0};
    void notifyDiagramChanged();
    ViewportRect
      rectAroundTextObject(const ViewportTextObject &text_object) const;
    void checkDiagramStateIsCompatibleWithTheDiagram();

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
    bool
      nodeOutputContains(
        int node_index,
        int output_index,
        const ViewportPoint &p
      );
    bool
      nodeInputContains(int node_index,int input_index,const ViewportPoint &);
    NodeConnectorIndex indexOfNodeConnectorContaining(const ViewportPoint &);

    ViewportSize textSize(const std::string &text) const;

    ViewportTextObject
      inputTextObject(const std::string &s,float left_x,float y) const;

    ViewportRect nodeHeaderRect(const DiagramTextObject &text_object) const;
    bool
      nodeContains(
        NodeIndex node_index,
        const ViewportPoint &p
      ) const;
    NodeIndex indexOfNodeContaining(const ViewportPoint &p) const;
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

    struct ClosestColumnResult {
      int column_index;
      float distance;
      float vertical_distance;
    };

    int
      closestColumn(
        const ViewportTextObject &line_text_object,
        const ViewportPoint &
      ) const;

    ClosestColumnResult
      closestColumn2(
        const ViewportTextObject &line_text_object,
        const ViewportPoint &
      ) const;

    NodeTextEditor::CursorPosition
      closestCursorPositionTo(
        NodeIndex node_index,
        const ViewportPoint &p
      ) const;

    void setDiagramPtr(Diagram *);
    void setDiagramStatePtr(const DiagramEvaluationState *);

    std::vector<NodeIndex> selected_node_indices;
    bool node_was_selected = false;
    ViewportPoint mouse_press_position;
    std::map<NodeIndex,DiagramPoint> original_node_positions;
    MouseMode mouse_mode = MouseMode::none;
    ViewportVector mouse_down_view_offset;
};

#endif /* DIAGRAMEDITOR_HPP_ */
