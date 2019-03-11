#ifndef FAKEDIAGRAMEDITOR_HPP
#define FAKEDIAGRAMEDITOR_HPP

#include "diagrameditor.hpp"
#include "optional.hpp"
#include "viewportline.hpp"


struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;
  Optional<std::string> maybe_chosen_path;
  bool an_error_was_shown = false;

  FakeDiagramEditor()
  {
  }

  int userAddsANode()
  {
    return userAddsANodeWithText("");
  }

  NodeIndex
    userAddsANodeWithTextAt(
      const std::string &text,
      const DiagramPoint &position
    )
  {
    return addNode(text,position);
  }

  int userAddsANodeWithText(const std::string &text)
  {
    return userAddsANodeWithTextAt(text,DiagramPoint(0,0));
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

  void userPressesMouseAt(const ViewportPoint &p)
  {
    leftMousePressedAt(ViewportPoint(p),EventModifiers());
  }

  void
    userPressesMiddleMouseAt(
      const ViewportPoint &p,
      const EventModifiers &modifiers
    )
  {
    middleMousePressedAt(p,modifiers);
  }

  void userMovesMouseTo(const ViewportPoint &p)
  {
    mouseMovedTo(p);
  }

  void userReleasesMouseAt(const ViewportPoint &p)
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

  NodeTextEditor::CursorPosition cursorPosition() const
  {
    return text_editor.cursorPosition();
  }

  static EventModifiers ctrlModifier()
  {
    EventModifiers result;
    result.ctrl_is_pressed = true;
    return result;
  }

  void userCtrlPressesLeftMouseAt(const ViewportPoint &p)
  {
    leftMousePressedAt(p,ctrlModifier());
  }

  void userClicksAt(const ViewportPoint &p)
  {
    leftMousePressedAt(p,EventModifiers());
    mouseReleasedAt(p);
  }

  void userClicksOnNode(NodeIndex n)
  {
    userClicksAt(nodeCenter(n));
  }

  ViewportPoint viewportCoordsForCenterOfNode(NodeIndex n)
  {
    return nodeCenter(n);
  }

  void userClicksWithShiftPressedAt(const ViewportPoint &p)
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

  NodeIndex focusedNodeIndex() const { return focused_node_index; }

  void userPressesEnter()
  {
    enterPressed();
  }

  void userPressesBackspace()
  {
    backspacePressed();
  }

  void userPressesDelete()
  {
    deletePressed();
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

  ViewportVector viewOffset() const { return view_offset; }

  ViewportRect rectAroundText(const std::string &) const override;

  int characterHeight() const { return 10; }

  ViewportLine
    textObjectCursorLine(
      const ViewportTextObject &/*text_object*/,
      int /*column_index*/
    ) const override;

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

  ViewportPoint nodeCenter(NodeIndex node_index)
  {
    return nodeRenderInfo(diagram().node(node_index)).body_outer_rect.center();
  }

  ViewportPoint nodeInputPosition(NodeIndex node_index,int output_index)
  {
    return nodeInputCircle(node(node_index),output_index).center;
  }

  ViewportPoint nodeOutputPosition(NodeIndex node_index,int output_index)
  {
    return nodeOutputCircle(node(node_index),output_index).center;
  }

  void callDiagramChangedCallback()
  {
    notifyDiagramChanged();
  }

  using DiagramEditor::aNodeIsFocused;
  using DiagramEditor::nodeRenderInfo;
  using DiagramEditor::nSelectedNodes;
  using DiagramEditor::nodeIsSelected;
  using DiagramEditor::viewportCoordsFromDiagramCoords;
  using DiagramEditor::cursorLine;
};


#endif /* FAKEDIAGRAMEDITOR_HPP */
