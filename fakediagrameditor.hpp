#ifndef FAKEDIAGRAMEDITOR_HPP
#define FAKEDIAGRAMEDITOR_HPP

#include "diagrameditor.hpp"


struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;

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

  int userAddsANodeWithTextAt(const std::string &text,const Point2D &position)
  {
    return addNode(text,position);
  }

  int userAddsANodeWithText(const std::string &text)
  {
    return userAddsANodeWithTextAt(text,Point2D(0,0));
  }

  void userPressesMouseAt(const Point2D &p)
  {
    mousePressedAt(p,/*shift_is_pressed*/false);
  }

  void userMovesMouseTo(const Point2D &p)
  {
    mouseMovedTo(p);
  }

  void userRelasesMouseAt(const Point2D &p)
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

  void userClicksAt(const Point2D &p)
  {
    mousePressedAt(p,/*shift_is_pressed*/false);
    mouseReleasedAt(p);
  }

  void userClicksWithShiftPressedAt(const Point2D &p)
  {
    mousePressedAt(p,/*shift_is_pressed*/true);
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

  virtual void redraw()
  {
    ++redraw_count;
  }

  virtual Rect rectAroundText(const TextObject &text_object) const
  {
    // Just make all text objects fit in a 10x10 square for now.
    Point2D begin_pos = text_object.position;
    Point2D end_pos = begin_pos;
    end_pos.x += 10;
    end_pos.y += 10;
    return Rect{begin_pos,end_pos};
  }

  Point2D nodeCenter(NodeIndex node_index)
  {
    return nodeRenderInfo(diagram().node(node_index)).body_outer_rect.center();
  }

  using DiagramEditor::aNodeIsFocused;
  using DiagramEditor::nodeRenderInfo;
  using DiagramEditor::nSelectedNodes;
  using DiagramEditor::nodeIsSelected;
};


#endif /* FAKEDIAGRAMEDITOR_HPP */
