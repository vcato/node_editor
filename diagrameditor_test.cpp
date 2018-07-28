#include "diagrameditor.hpp"

#include <cassert>
#include "fakediagrameditor.hpp"


using std::string;
using std::cerr;


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
  int diagram_changed_count = 0;
  editor.diagramChangedCallback() = [&](){ ++diagram_changed_count; };

  editor.userPressesBackspace();
  assert(diagram.nExistingNodes()==0);
  assert(diagram_changed_count==1);
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


static void testChangingText4()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int diagram_changed_count = 0;
  editor.diagramChangedCallback() = [&](){ ++diagram_changed_count; };
  NodeIndex n1 = editor.userAddsANodeWithText("return 0");
  editor.userFocusesNode(n1);
  editor.userMovesCursorTo(/*row*/0,/*column*/8);
  diagram_changed_count = 0;
  editor.userPressesBackspace();
  assert(diagram_changed_count==1);
  editor.userTypesText("1");
  assert(diagram_changed_count==2);
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


static void testCancellingExport()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  editor.userPressesExportDiagram(/*chosen_path*/"");
  assert(!editor.an_error_was_shown);
}


#if 0
static void testImportingDiagram()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int diagram_changed_count = 0;
  editor.diagramChangedCallback() = [&]{ ++diagram_changed_count; };
  editor.userPressesImportDiagram(/*chosen_path*/"test.dat");
  assert(!editor.an_error_was_shown);
  assert(diagram_changed_count==1);
}
#endif


static bool
  diagramHasConnection(
    const Diagram &diagram,
    NodeIndex node1_index,
    int output_number,
    const NodeIndex node2_index,
    int input_number
  )
{
  DiagramNode::Input node2_input =
    diagram.node(node2_index).inputs[input_number];
  return node2_input == DiagramNode::Input{node1_index,output_number};
}


static void testConnectingNodes()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex node1 = editor.userAddsANodeWithTextAt("1",Point2D(0,0));
  NodeIndex node2 = editor.userAddsANodeWithTextAt("return $",Point2D(100,0));
  editor.userPressesMouseAt(editor.nodeOutputPosition(node1,0));

  int diagram_change_count = 0;
  editor.diagramChangedCallback() = [&]{ ++diagram_change_count; };
  editor.userRelasesMouseAt(editor.nodeInputPosition(node2,0));
  assert(diagramHasConnection(diagram,node1,0,node2,0));
  assert(diagram_change_count==1);
}


int main()
{
  test1();
  testDeletingANode();
  testDeletingAConnectedNode();
  testChangingText();
  testChangingText2();
  testChangingText3();
  testChangingText4();
  testSettingDiagramPtr();
  testSettingDiagramPtrWithAnEmptyFocusedNode();
  testClickingOnBackgroundTwice();
  testEscape();
  test2();
  testRenderInfo();
  testSelectingMultipleNodes();
  testCancellingExport();
  testConnectingNodes();
  // testImportingDiagram();
}
