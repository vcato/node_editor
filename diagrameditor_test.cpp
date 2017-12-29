#include "diagrameditor.hpp"

#include <cassert>


using std::string;
using std::cerr;


namespace {
struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;

  FakeDiagramEditor(Diagram &diagram)
  : DiagramEditor()
  {
    setDiagramPtr(&diagram);
  }

  int userAddsANode()
  {
    return userAddsANodeWithText("");
  }

  int userAddsANodeWithTextAt(const string &text,const Point2D &position)
  {
    return addNode(text,position);
  }

  int userAddsANodeWithText(const string &text)
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
}


static void test1()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);

  // User clicks on the background.
  // A single node exists.
  // The node is focused.
}


static void testDeletingANode()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int node_index = editor.userAddsANodeWithText("test");
  editor.userSelectsNode(node_index);
  editor.userPressesBackspace();
  assert(diagram.nExistingNodes()==0);
}


static void testDeletingAConnectedNode()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int n1 = editor.userAddsANodeWithText("5");
  int n2 = editor.userAddsANodeWithText("a=$");
  editor.userConnects(n1,0,n2,0);
  editor.userSelectsNode(n1);
  editor.userPressesBackspace();
  assert(diagram.node(n2).inputs[0].source_node_index==nullNodeIndex());
}


static void testChangingText()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int n1 = editor.userAddsANodeWithText("5");
  int n2 = editor.userAddsANodeWithText("show($)");
  editor.userConnects(n1,0,n2,0);
  editor.userFocusesNode(n1);
  editor.userPressesBackspace();
  editor.userUnfocusesNode();
  assert(diagram.node(n2).inputs[0].source_node_index<0);
}


static void testChangingText2()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int n1 = editor.userAddsANode();
  editor.userFocusesNode(n1);
  editor.userTypesText("$");
  assert(diagram.node(n1).statements[0].has_output);
  editor.userPressesBackspace();

  // Once a line gets an output, we want it to keep the output until
  // we move to another line to avoid disconnections due to temporary
  // text changes.
  assert(diagram.node(n1).statements[0].has_output);

  editor.userTypesText("x=");

  assert(diagram.node(n1).statements[0].has_output);
}


static void testChangingText3()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex n1 = editor.userAddsANodeWithText("x\ny\nz");
  NodeIndex n2 = editor.userAddsANodeWithText("$");
  editor.userConnects(n1,2,n2,0);
  editor.userFocusesNode(n1);
  editor.userMovesCursorTo(/*row*/2,/*column*/0);
  editor.userPressesBackspace();
  assert(diagram.node(n1).nOutputs()==2);
  assert(diagram.node(n2).inputs[0].source_output_index!=2);
}


static void testSettingDiagramPtr()
{
  {
    Diagram diagram;
    FakeDiagramEditor editor(diagram);
    NodeIndex n1 = editor.userAddsANode();
    editor.userFocusesNode(n1);
    editor.setDiagramPtr(0);
    assert(!editor.aNodeIsFocused());
  }
  {
    Diagram diagram;
    FakeDiagramEditor editor(diagram);
    editor.setDiagramPtr(0);
  }
  {
    Diagram diagram;
    FakeDiagramEditor editor(diagram);
    NodeIndex n1 = editor.userAddsANode();
    editor.userSelectsNode(n1);
    editor.setDiagramPtr(0);
    assert(editor.nSelectedNodes()==0);
  }
}


static void testSettingDiagramPtrWithAnEmptyFocusedNode()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  editor.userClicksAt(Point2D(100,100));
  editor.setDiagramPtr(0);
  assert(diagram.existingNodeIndices().empty());
}


static void testClickingOnBackgroundTwice()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  editor.userClicksAt(Point2D(100,100));
  editor.userClicksAt(Point2D(200,200));
}


static void testEscape()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  editor.userClicksAt(Point2D(100,100));
  int n_redraws = editor.redraw_count;
  editor.userPressesEscape();
  assert(!editor.aNodeIsFocused());
  assert(diagram.nExistingNodes()==0);
  assert(editor.redraw_count==n_redraws+1);
}


static void test2()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex n1 = editor.userAddsANodeWithText("x");
  NodeIndex n2 = editor.userAddsANodeWithText("$");
  editor.userConnects(n1,0,n2,0);
  editor.userFocusesNode(n1);
  editor.userPressesBackspace();
  editor.userPressesEnter();
  assert(diagram.node(n2).inputs[0].source_node_index==nullNodeIndex());
}


static void testRenderInfo()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex n1 = editor.userAddsANodeWithText("$+$");
  DiagramNode &node = diagram.node(n1);
  auto render_info = editor.nodeRenderInfo(node);
  int n_input_circles = render_info.input_connector_circles.size();
  assert(n_input_circles==node.nInputs());
}


static void testSelectingMultipleNodes()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex n1 = editor.userAddsANodeWithTextAt("test",Point2D(0,0));
  NodeIndex n2 = editor.userAddsANodeWithTextAt("test",Point2D(0,100));
  editor.userClicksAt(editor.nodeCenter(n1));
  editor.userClicksWithShiftPressedAt(editor.nodeCenter(n2));
  assert(editor.nSelectedNodes()==2);
  assert(editor.nodeIsSelected(n1));
  assert(editor.nodeIsSelected(n2));
  Point2D mouse_down_position = editor.nodeCenter(n1);
  editor.userPressesMouseAt(mouse_down_position);
  assert(editor.nSelectedNodes()==2);
  Point2D mouse_release_position =
    Point2D(mouse_down_position.x+10,mouse_down_position.y);
  editor.userMovesMouseTo(mouse_release_position);
  assert(diagram.node(n1).position()==Point2D(10,0));
  assert(diagram.node(n2).position()==Point2D(10,100));
  editor.userRelasesMouseAt(mouse_release_position);
  editor.userPressesBackspace();
  assert(diagram.nExistingNodes()==0);
}


int main()
{
  test1();
  testDeletingANode();
  testDeletingAConnectedNode();
  testChangingText();
  testChangingText2();
  testChangingText3();
  testSettingDiagramPtr();
  testSettingDiagramPtrWithAnEmptyFocusedNode();
  testClickingOnBackgroundTwice();
  testEscape();
  test2();
  testRenderInfo();
  testSelectingMultipleNodes();
}
