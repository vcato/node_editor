#include "diagrameditor.hpp"

#include <cassert>


using std::string;


namespace {
struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;

  FakeDiagramEditor(Diagram &diagram) : DiagramEditor(diagram) { }

  int userAddsANode()
  {
    return userAddsANodeWithText("");
  }

  int userAddsANodeWithText(const string &text)
  {
    return addNode(text,Point2D(0,0));
  }

  void userSelectsNode(int node_index)
  {
    node_editor.selectNode(node_index);
  }

  void userFocusesNode(int node_index)
  {
    node_editor.focusNode(node_index,diagram());
  }

  void userMovesCursorTo(int line,int column)
  {
    node_editor.text_editor.moveCursor(line,column);
  }

  void userClicksAt(const Point2D &p)
  {
    mousePressedAt(p);
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

  using DiagramEditor::aNodeIsFocused;
  using DiagramEditor::nodeRenderInfo;
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


int main()
{
  test1();
  testDeletingANode();
  testDeletingAConnectedNode();
  testChangingText();
  testChangingText2();
  testSettingDiagramPtr();
  testSettingDiagramPtrWithAnEmptyFocusedNode();
  testClickingOnBackgroundTwice();
  test2();
  testRenderInfo();
}
