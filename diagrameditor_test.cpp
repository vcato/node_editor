#include "diagrameditor.hpp"

#include <unistd.h>
#include <cassert>
#include <fstream>
#include "fakediagrameditor.hpp"

using std::string;
using std::cerr;
using std::ofstream;



static const char *empty_diagram_text =
  "diagram {\n"
  "}\n";


static const char *bad_diagram_text =
  "diagram{\n"
  "}\n";


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
  editor.userClicksAt(ViewportCoords(100,100));
  editor.setDiagramPtr(0);
  assert(diagram.existingNodeIndices().empty());
}


static void testClickingOnBackgroundTwice()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  editor.redraw_count = 0;
  editor.userClicksAt(ViewportCoords(100,100));
  assert(editor.redraw_count==2);
  editor.userClicksAt(ViewportCoords(200,200));
  assert(editor.redraw_count==4);
}


static void testEscape()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  editor.userClicksAt(ViewportCoords(100,100));
  assert(diagram.nNodes()==1);
  int n_redraws = editor.redraw_count;
  editor.userPressesEscape();
  assert(!editor.aNodeIsFocused());
  assert(diagram.nExistingNodes()==0);
  assert(editor.redraw_count==n_redraws+1);
}


static void testTypingInNode()
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


static void testClickingOnANode()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex n1 = editor.userAddsANodeWithText("test");

  editor.userClicksOnNode(n1);

  assert(editor.nodeIsSelected(n1));
  assert(diagram.nNodes()==1);
}


static void testShiftSelectingMultipleNodes()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex n1 = editor.userAddsANodeWithTextAt("test",DiagramCoords(0,0));
  NodeIndex n2 = editor.userAddsANodeWithTextAt("test",DiagramCoords(0,100));

  editor.userClicksOnNode(n1);

  editor.userClicksWithShiftPressedAt(
    editor.viewportCoordsFromDiagramCoords(editor.nodeCenter(n2))
  );

  assert(editor.nSelectedNodes()==2);
  assert(editor.nodeIsSelected(n1));
  assert(editor.nodeIsSelected(n2));

  ViewportCoords mouse_down_position =
    editor.viewportCoordsFromDiagramCoords(editor.nodeCenter(n1));

  editor.userPressesMouseAt(mouse_down_position);
  assert(editor.nSelectedNodes()==2);
  ViewportCoords mouse_release_position =
    ViewportCoords(mouse_down_position.x+10,mouse_down_position.y);
  editor.userMovesMouseTo(mouse_release_position);
  assert(diagram.node(n1).position()==Point2D(10,0));
  assert(diagram.node(n2).position()==Point2D(10,100));
  editor.userReleasesMouseAt(mouse_release_position);
  editor.userPressesBackspace();
  assert(diagram.nExistingNodes()==0);
}


static void
  testRectangleSelectingMultipleNodes(
    const ViewportCoords &start,
    const ViewportCoords &end
  )
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex n1 = editor.userAddsANodeWithTextAt("x",DiagramCoords(10,10));
  NodeIndex n2 = editor.userAddsANodeWithTextAt("y",DiagramCoords(20,20));

  editor.userPressesMouseAt(start);
  editor.userMovesMouseTo(end);
  editor.userReleasesMouseAt(end);

  assert(editor.nodeIsSelected(n1));
  assert(editor.nodeIsSelected(n2));
}


static void testRectangleSelectingMultipleNodes1()
{
  testRectangleSelectingMultipleNodes(
    /*start*/ViewportCoords(0,0),/*end*/ViewportCoords(40,40)
  );
}


static void testRectangleSelectingMultipleNodes2()
{
  testRectangleSelectingMultipleNodes(
    /*start*/ViewportCoords(40,40),/*end*/ViewportCoords(0,0)
  );
}


static void testTranslatingView()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  EventModifiers modifiers;
  modifiers.alt_is_pressed = true;
  editor.userPressesMiddleMouseAt(ViewportCoords(10,10),modifiers);
  editor.userMovesMouseTo(ViewportCoords(20,10));
  assert(editor.viewOffset()==Vector2D(10,0));
}


static void testTranslatingView2()
{
  Diagram diagram;
  NodeIndex node_index = diagram.createNodeWithText("test");
  FakeDiagramEditor editor(diagram);
  EventModifiers modifiers;
  modifiers.alt_is_pressed = true;
  editor.userPressesMiddleMouseAt(ViewportCoords(10,10),modifiers);

  NodeRenderInfo orig_render_info =
    editor.nodeRenderInfo(diagram.node(node_index));

  editor.userMovesMouseTo(ViewportCoords(20,10));

  assert(editor.viewOffset()==Vector2D(10,0));
  NodeRenderInfo translated_render_info =
    editor.nodeRenderInfo(diagram.node(node_index));

  assert(
    translated_render_info.header_rect.start ==
    orig_render_info.header_rect.start + Vector2D(10,0)
  );
}


static void testCancellingExport()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  editor.userPressesExportDiagram(/*chosen_path*/"");
  assert(!editor.an_error_was_shown);
}


static void saveFile(const char *path,const char *text)
{
  ofstream stream(path);
  assert(!!stream);
  stream << text;
}


namespace {
class ImportTester {
  public:
    ImportTester()
    {
      editor.diagramChangedCallback() = [&]{ ++diagram_changed_count; };
    }

    ~ImportTester()
    {
      unlink(test_diagram_path);
    }

    void runWithEmptyDiagram()
    {
      userImportsDiagramText(empty_diagram_text);
      assert(!editor.an_error_was_shown);
      assert(diagram_changed_count==1);
    }

    void runWithBadDiagram()
    {
      userImportsDiagramText(bad_diagram_text);
      assert(editor.an_error_was_shown);
      assert(diagram_changed_count==0);
    }

    void runWithExistingNodes()
    {
      const char *text =
        "diagram {\n"
        "  node {\n"
        "    id: 1\n"
        "    position {\n"
        "      x: 275\n"
        "      y: 198\n"
        "    }\n"
        "    line: \"5\"\n"
        "  }\n"
        "}\n";

      userImportsDiagramText(text);
      assert(!editor.an_error_was_shown);
      userImportsDiagramText(text);
      assert(!editor.an_error_was_shown);
    }

  private:
    Diagram diagram;
    FakeDiagramEditor editor{diagram};
    int diagram_changed_count = 0;
    const char *test_diagram_path = "diagrameditortest.dat";

    void userImportsDiagramText(const char *text)
    {
      saveFile(test_diagram_path,text);
      editor.userPressesImportDiagram(test_diagram_path);
    }
};
}


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
  NodeIndex node1 =
    editor.userAddsANodeWithTextAt("1",DiagramCoords(0,0));
  NodeIndex node2 =
    editor.userAddsANodeWithTextAt("return $",DiagramCoords(100,0));
  editor.userPressesMouseAt(editor.nodeOutputPosition(node1,0));

  int diagram_change_count = 0;
  editor.diagramChangedCallback() = [&]{ ++diagram_change_count; };
  editor.userReleasesMouseAt(
    editor.viewportCoordsFromDiagramCoords(editor.nodeInputPosition(node2,0))
  );
  assert(diagramHasConnection(diagram,node1,0,node2,0));
  assert(diagram_change_count==1);
}


static ViewportCoords centerOf(const ViewportLine &l)
{
  return l.start + (l.end - l.start)/2;
}


static void testClickingOnAFocusedNode()
{
  // Have a diagram with a single node.
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex node = editor.userAddsANodeWithTextAt("12",DiagramCoords(0,0));

  // Click on the node to select it.
  editor.userClicksOnNode(node);
  assert(editor.nodeIsSelected(node));

  // Click again on the node to focus it.
  editor.userClicksOnNode(node);
  assert(editor.aNodeIsFocused());

  // Click again on the node to move the cursor.
  NodeTextEditor::CursorPosition desired_cursor_position{/*line*/0,/*column*/1};
  ViewportLine cursor_line =
    editor.cursorLine(editor.focusedNodeIndex(),desired_cursor_position);
  editor.userClicksAt(centerOf(cursor_line));

  // The cursor position should have changed.
  assert(editor.cursorPosition() == desired_cursor_position);
  assert(editor.aNodeIsFocused());
}


static void testClickingOnAFocusedNode2()
{
  string node_text = "abcd\ne";

  // Have a diagram with a single node.
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  NodeIndex node = editor.userAddsANodeWithTextAt(node_text,DiagramCoords(0,0));

  // Click on the node to select it.
  editor.userClicksOnNode(node);

  // Click again on the node to focus it.
  editor.userClicksOnNode(node);

  // Click again below the d
  using CursorPosition = NodeTextEditor::CursorPosition;
  CursorPosition d_cursor_position{/*line*/0,/*column*/3};
  ViewportLine cursor_line =
    editor.cursorLine(editor.focusedNodeIndex(),d_cursor_position);
  assert(cursor_line.start.y < cursor_line.end.y);
  auto character_height = editor.characterHeight();
  ViewportCoords p = cursor_line.start - Vector2D(0,character_height/2);

  int old_redraw_count = editor.redraw_count;

  editor.userClicksAt(editor.viewportCoordsFromDiagramCoords(p));

  // The cursor position should have changed.
  CursorPosition expected_cursor_position(/*line*/1,/*column*/1);
  assert(editor.cursorPosition() == expected_cursor_position);
  assert(editor.redraw_count == old_redraw_count+1);
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
  testTypingInNode();
  testRenderInfo();
  testClickingOnANode();
  testShiftSelectingMultipleNodes();
  testRectangleSelectingMultipleNodes1();
  testRectangleSelectingMultipleNodes2();
  testTranslatingView();
  testTranslatingView2();
  testCancellingExport();
  testConnectingNodes();
  testClickingOnAFocusedNode();
  testClickingOnAFocusedNode2();

  ImportTester().runWithEmptyDiagram();
  ImportTester().runWithBadDiagram();
  ImportTester().runWithExistingNodes();
}
