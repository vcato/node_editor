#ifndef FAKEDIAGRAMEDITOR_HPP
#define FAKEDIAGRAMEDITOR_HPP

#include "diagrameditor.hpp"
#include "optional.hpp"


struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;
  Optional<std::string> maybe_chosen_path;
  bool an_error_was_shown = false;

  FakeDiagramEditor()
  {
  }

  FakeDiagramEditor(Diagram &diagram)
  : DiagramEditor()
  {
    setDiagramPtr(&diagram);
  }

  int userAddsANode()
  {
    return userAddsANodeWithText("");
  }

  NodeIndex
    userAddsANodeWithTextAt(const std::string &text,const Point2D &position)
  {
    return addNode(text,position);
  }

  int userAddsANodeWithText(const std::string &text)
  {
    return userAddsANodeWithTextAt(text,Point2D(0,0));
  }

  void userPressesExportDiagram(const std::string &chosen_path)
  {
    maybe_chosen_path = chosen_path;
    exportDiagramPressed();
    maybe_chosen_path.reset();
  }

  void userPressesImportDiagram(const std::string &chosen_path)
  {
    maybe_chosen_path = chosen_path;
    importDiagramPressed();
    maybe_chosen_path.reset();
  }

  void userPressesMouseAt(const Point2D &p)
  {
    leftMousePressedAt(ViewportCoords(p),EventModifiers());
  }

  void
    userPressesMiddleMouseAt(
      const ViewportCoords &p,
      const EventModifiers &modifiers
    )
  {
    middleMousePressedAt(p,modifiers);
  }

  void userMovesMouseTo(const ViewportCoords &p)
  {
    mouseMovedTo(p);
  }

  void userReleasesMouseAt(const ViewportCoords &p)
  {
    mouseReleasedAt(p);
  }

  void userSelectsNode(int node_index)
  {
    selectNode(node_index);
  }

  void userAlsoSelectsNode(NodeIndex node_index)
  {
    alsoSelectNode(node_index);
  }

  void userFocusesNode(int node_index)
  {
    focusNode(node_index,diagram());
  }

  void userMovesCursorTo(int line,int column)
  {
    text_editor.moveCursor(line,column);
  }

  void userClicksAt(const ViewportCoords &p)
  {
    leftMousePressedAt(p,EventModifiers());
    mouseReleasedAt(p);
  }

  void userClicksWithShiftPressedAt(const ViewportCoords &p)
  {
    EventModifiers modifiers;
    modifiers.shift_is_pressed = true;
    leftMousePressedAt(p,modifiers);
    mouseReleasedAt(p);
  }

  void userUnfocusesNode()
  {
    unfocus();
  }

  void
    userConnects(
      int output_node_index,
      int output_index,
      int input_node_index,
      int input_index
    )
  {
    connectNodes(
      output_node_index,output_index,
      input_node_index,input_index
    );
  }

  void userPressesEnter()
  {
    enterPressed();
  }

  void userPressesBackspace()
  {
    backspacePressed();
  }

  void userPressesEscape()
  {
    escapePressed();
  }

  void userTypesText(const std::string &text)
  {
    textTyped(text);
  }

  void redraw() override
  {
    ++redraw_count;
  }

  Vector2D viewOffset() const { return view_offset; }

  Rect rectAroundText(const TextObject &text_object) const override
  {
    // Just make all text objects fit in a 10x10 square for now.
    Point2D begin_pos = text_object.position;
    Point2D end_pos = begin_pos;
    end_pos.x += 10;
    end_pos.y += 10;
    return Rect{begin_pos,end_pos};
  }

  std::string askForSavePath() override
  {
    return *maybe_chosen_path;
  }

  std::string askForOpenPath() override
  {
    return *maybe_chosen_path;
  }

  void showError(const std::string &/*message*/) override
  {
    an_error_was_shown = true;
  }

  Point2D nodeCenter(NodeIndex node_index)
  {
    return nodeRenderInfo(diagram().node(node_index)).body_outer_rect.center();
  }

  Point2D nodeInputPosition(NodeIndex node_index,int output_index)
  {
    return nodeInputCircle(node(node_index),output_index).center;
  }

  Point2D nodeOutputPosition(NodeIndex node_index,int output_index)
  {
    return nodeOutputCircle(node(node_index),output_index).center;
  }

  using DiagramEditor::aNodeIsFocused;
  using DiagramEditor::nodeRenderInfo;
  using DiagramEditor::nSelectedNodes;
  using DiagramEditor::nodeIsSelected;
  using DiagramEditor::viewportCoordsFromDiagramCoords;
  using DiagramEditor::ViewportCoords;
};


#endif /* FAKEDIAGRAMEDITOR_HPP */
