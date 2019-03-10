#include "diagrameditor.hpp"

#include <unistd.h>
#include <cassert>
#include <fstream>
#include "fakediagrameditor.hpp"

using std::string;
using std::cerr;
using std::ofstream;
using std::make_unique;
using std::function;



static const char *empty_diagram_text =
  "diagram {\n"
  "}\n";


static const char *bad_diagram_text =
  "diagram{\n"
  "}\n";


namespace {
struct TestHolder : ObservedDiagram::Holder {
  function<void()> diagram_changed_callback;

  void notifyDiagramUnobserved(Diagram &) override
  {
  }

  void notifyDiagramChanged(Diagram &) override
  {
    if (diagram_changed_callback) {
      diagram_changed_callback();
    }
  }
};
}


namespace {
struct Tester {
  Diagram diagram;

  TestHolder holder;
  ObservedDiagram observed_diagram{diagram,holder};
  FakeDiagramEditor editor;

  Tester()
  {
    editor.setDiagramObserver(
      make_unique<ObservedDiagram::Observer>(
        observed_diagram,
        /*diagram_changed_hook*/[](){}
      )
    );
  }

  auto &diagramChangedCallback() { return holder.diagram_changed_callback; }
};
}


static void testDeletingANode()
{
  Tester tester;
  Diagram &diagram = tester.diagram;
  FakeDiagramEditor &editor = tester.editor;
  int node_index = editor.userAddsANodeWithText("test");
  editor.userSelectsNode(node_index);

  int diagram_changed_count = 0;
  auto diagram_changed_function = [&](){ ++diagram_changed_count; };
  tester.diagramChangedCallback() = diagram_changed_function;

  editor.userPressesBackspace();
  assert(diagram.nExistingNodes()==0);
  assert(diagram_changed_count==1);
}


static void testDeletingAConnectedNode()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
  int n1 = editor.userAddsANodeWithText("5");
  int n2 = editor.userAddsANodeWithText("a=$");
  editor.userConnects(n1,0,n2,0);
  editor.userSelectsNode(n1);
  editor.userPressesBackspace();
  assert(diagram.node(n2).inputs[0].source_node_index==nullNodeIndex());
}


static void testChangingText()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
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
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
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
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
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
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  int diagram_changed_count = 0;
  tester.diagramChangedCallback() = [&](){ ++diagram_changed_count; };
  NodeIndex n1 = editor.userAddsANodeWithText("return 0");
  editor.userFocusesNode(n1);
  editor.userMovesCursorTo(/*row*/0,/*column*/8);
  diagram_changed_count = 0;
  editor.userPressesBackspace();
  assert(diagram_changed_count==1);
  editor.userTypesText("1");
  assert(diagram_changed_count==2);
}


void testChangingText5()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  int diagram_changed_count = 0;
  tester.diagramChangedCallback() = [&](){ ++diagram_changed_count; };
  NodeIndex n1 = editor.userAddsANodeWithText("return 10");
  editor.userFocusesNode(n1);
  editor.userMovesCursorTo(/*row*/0,/*column*/8);
  diagram_changed_count = 0;
  editor.userPressesDelete();
  assert(diagram_changed_count==1);
}


static void testSettingDiagramPtr()
{
  {
    Tester tester;
    FakeDiagramEditor &editor = tester.editor;
    NodeIndex n1 = editor.userAddsANode();
    editor.userFocusesNode(n1);
    editor.setDiagramObserver(0);
    assert(!editor.aNodeIsFocused());
  }
  {
    Tester tester;
    tester.editor.setDiagramObserver(0);
  }
  {
    Tester tester;
    FakeDiagramEditor &editor = tester.editor;
    NodeIndex n1 = editor.userAddsANode();
    editor.userSelectsNode(n1);
    editor.setDiagramObserver(0);
    assert(editor.nSelectedNodes()==0);
  }
}


static void testSettingDiagramPtrWithAnEmptyFocusedNode()
{
  Tester tester;
  Diagram &diagram = tester.diagram;
  FakeDiagramEditor &editor = tester.editor;
  editor.userClicksAt(ViewportCoords(100,100));
  editor.setDiagramObserver(0);
  assert(diagram.existingNodeIndices().empty());
}


static void testClickingOnBackgroundTwice()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  editor.redraw_count = 0;
  editor.userClicksAt(ViewportCoords(100,100));
  assert(editor.redraw_count==2);
  editor.userClicksAt(ViewportCoords(200,200));
  assert(editor.redraw_count==4);
}


static void testEscapeWithAFocusedEmptyNode()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
  editor.userClicksAt(ViewportCoords(100,100));
  assert(diagram.nNodes()==1);
  int n_redraws = editor.redraw_count;
  editor.userPressesEscape();
  assert(!editor.aNodeIsFocused());
  assert(diagram.nExistingNodes()==0);
  assert(editor.redraw_count==n_redraws+1);
}


static void testEscapeWithAFocusedNonEmptyNode()
{
  Tester tester;
  Diagram &diagram = tester.diagram;

  NodeIndex n = diagram.createNodeWithText("12");
  FakeDiagramEditor &editor = tester.editor;
  editor.userFocusesNode(n);
  editor.userPressesEscape();
  assert(!editor.aNodeIsFocused());
  assert(editor.nodeIsSelected(n));
}


static void testTypingInNode()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
  NodeIndex n1 = editor.userAddsANodeWithText("x");
  NodeIndex n2 = editor.userAddsANodeWithText("$");

  int diagram_changed_count = 0;
  auto diagram_changed_function = [&](){ ++diagram_changed_count; };
  tester.diagramChangedCallback() = diagram_changed_function;

  editor.userConnects(n1,0,n2,0);
  editor.userFocusesNode(n1);
  editor.userPressesBackspace();

  diagram_changed_count = 0;

  editor.userPressesEnter();

  assert(diagram.node(n2).inputs[0].source_node_index == nullNodeIndex());
  assert(diagram_changed_count == 1);
}


static void testRenderInfo()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
  NodeIndex n1 = editor.userAddsANodeWithText("$+$");
  DiagramNode &node = diagram.node(n1);
  auto render_info = editor.nodeRenderInfo(node);
  int n_input_circles = render_info.input_connector_circles.size();
  assert(n_input_circles==node.nInputs());
}


static void testClickingOnANode()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
  NodeIndex n1 = editor.userAddsANodeWithText("test");

  editor.userClicksOnNode(n1);

  assert(editor.nodeIsSelected(n1));
  assert(diagram.nNodes()==1);
}


static void testShiftSelectingMultipleNodes()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
  NodeIndex n1 = editor.userAddsANodeWithTextAt("test",DiagramCoords(0,0));
  NodeIndex n2 = editor.userAddsANodeWithTextAt("test",DiagramCoords(0,100));

  editor.userClicksOnNode(n1);

  editor.userClicksWithShiftPressedAt( editor.nodeCenter(n2) );

  assert(editor.nSelectedNodes()==2);
  assert(editor.nodeIsSelected(n1));
  assert(editor.nodeIsSelected(n2));

  ViewportCoords mouse_down_position = editor.nodeCenter(n1);

  editor.userPressesMouseAt(mouse_down_position);
  assert(editor.nSelectedNodes()==2);
  ViewportCoords mouse_release_position =
    ViewportCoords(mouse_down_position.x+10,mouse_down_position.y);
  editor.userMovesMouseTo(mouse_release_position);
  assert(diagram.node(n1).position()==DiagramCoords(10,0));
  assert(diagram.node(n2).position()==DiagramCoords(10,100));
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
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
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
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  EventModifiers modifiers;
  modifiers.alt_is_pressed = true;
  editor.userPressesMiddleMouseAt(ViewportCoords(10,10),modifiers);
  editor.userMovesMouseTo(ViewportCoords(20,10));
  assert(editor.viewOffset()==ViewportVector(10,0));
}


static void testTranslatingView2()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  Diagram &diagram = tester.diagram;
  NodeIndex node_index = diagram.createNodeWithText("test");
  EventModifiers modifiers;
  modifiers.alt_is_pressed = true;
  editor.userPressesMiddleMouseAt(ViewportCoords(10,10),modifiers);

  NodeRenderInfo orig_render_info =
    editor.nodeRenderInfo(diagram.node(node_index));

  editor.userMovesMouseTo(ViewportCoords(20,10));

  assert(editor.viewOffset()==ViewportVector(10,0));
  NodeRenderInfo translated_render_info =
    editor.nodeRenderInfo(diagram.node(node_index));

  assert(
    translated_render_info.header_rect.start ==
    orig_render_info.header_rect.start + ViewportVector(10,0)
  );
}


static void testCancellingExport()
{
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
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
      tester.diagramChangedCallback() = [&]{ ++diagram_changed_count; };
    }

    ~ImportTester()
    {
      unlink(test_diagram_path);
    }

    void runWithEmptyDiagram()
    {
      userImportsDiagramText(empty_diagram_text);
      assert(!editor().an_error_was_shown);
      assert(diagram_changed_count==1);
    }

    void runWithBadDiagram()
    {
      userImportsDiagramText(bad_diagram_text);
      assert(editor().an_error_was_shown);
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
      assert(!editor().an_error_was_shown);
      userImportsDiagramText(text);
      assert(!editor().an_error_was_shown);
    }

  private:
    Tester tester;
    FakeDiagramEditor &editor() { return tester.editor; }
    int diagram_changed_count = 0;
    const char *test_diagram_path = "diagrameditortest.dat";

    void userImportsDiagramText(const char *text)
    {
      saveFile(test_diagram_path,text);
      editor().userPressesImportDiagram(test_diagram_path);
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
  Tester tester;
  Diagram &diagram = tester.diagram;
  FakeDiagramEditor &editor = tester.editor;
  NodeIndex node1 =
    editor.userAddsANodeWithTextAt("1",DiagramCoords(0,0));
  NodeIndex node2 =
    editor.userAddsANodeWithTextAt("return $",DiagramCoords(100,0));
  editor.userPressesMouseAt(editor.nodeOutputPosition(node1,0));

  int diagram_change_count = 0;
  tester.diagramChangedCallback() = [&]{ ++diagram_change_count; };
  editor.userReleasesMouseAt( editor.nodeInputPosition(node2,0) );
  assert(diagramHasConnection(diagram,node1,0,node2,0));
  assert(diagram_change_count==1);
}


static ViewportCoords linePoint(const ViewportLine &l,float fraction)
{
  return l.start + (l.end - l.start) * fraction;
}


namespace {
struct ClickingOnAFocusedNodeTest {
  using CursorPosition = NodeTextEditor::CursorPosition;
  string node_text;
  CursorPosition target_cursor_position{/*line*/0,/*column*/1};
  float target_fraction = 0.5;
  CursorPosition expected_cursor_position{/*line*/0,/*column*/1};
};
}


static void testClickingOnAFocusedNode(const ClickingOnAFocusedNodeTest &test)
{
  // Have a diagram with a single node.
  Tester tester;
  FakeDiagramEditor &editor = tester.editor;
  NodeIndex node =
    editor.userAddsANodeWithTextAt(test.node_text,DiagramCoords(0,0));

  // Click on the node to select it.
  editor.userClicksOnNode(node);
  assert(editor.nodeIsSelected(node));

  // Click again on the node to focus it.
  editor.userClicksOnNode(node);
  assert(editor.aNodeIsFocused());

  // Click again on the node to move the cursor.
  ViewportLine cursor_line =
    editor.cursorLine(editor.focusedNodeIndex(),test.target_cursor_position);
  assert(cursor_line.start.y < cursor_line.end.y);

  int old_redraw_count = editor.redraw_count;

  editor.userClicksAt(linePoint(cursor_line,test.target_fraction));

  // The cursor position should have changed.
  assert(editor.cursorPosition() == test.expected_cursor_position);
  assert(editor.aNodeIsFocused());
  assert(editor.redraw_count == old_redraw_count+1);
}


static void testClickingOnAFocusedNode1()
{
  ClickingOnAFocusedNodeTest test;
  test.node_text = "12";
  test.target_cursor_position = {/*line*/0,/*column*/1};
  test.target_fraction = 0.5;
  test.expected_cursor_position = test.target_cursor_position;

  testClickingOnAFocusedNode(test);
}


static void testClickingOnAFocusedNode2()
{
  ClickingOnAFocusedNodeTest test;
  test.node_text = "abcd\ne";
  test.target_cursor_position = {/*line*/0,/*column*/3};
  test.target_fraction = -0.5;
  test.expected_cursor_position = {/*line*/1,/*column*/1};

  testClickingOnAFocusedNode(test);
}


static void testClickingOnAFocusedNodeAboveFirstLine()
{
  ClickingOnAFocusedNodeTest test;
  test.node_text = "$ + $";
  test.target_cursor_position = {/*line*/0,/*column*/1};
  test.target_fraction = 1.5;
  test.expected_cursor_position = {/*line*/0,/*column*/1};

  testClickingOnAFocusedNode(test);
}


static void testClickingOnAFocusedNodeBelowFirstLine()
{
  ClickingOnAFocusedNodeTest test;
  test.node_text = "$ + $";
  test.target_cursor_position = {/*line*/0,/*column*/1};
  test.target_fraction = -0.5;
  test.expected_cursor_position = {/*line*/0,/*column*/1};

  testClickingOnAFocusedNode(test);
}


static void testCopyingANodeByCtrlDrag()
{
  Tester tester;
  Diagram &diagram = tester.diagram;
  NodeIndex n = diagram.createNodeWithText("5");
  FakeDiagramEditor &editor = tester.editor;
  ViewportCoords press_position = editor.viewportCoordsForCenterOfNode(n);
  ViewportCoords release_position = press_position + ViewportVector(0,-20);

  editor.userCtrlPressesLeftMouseAt(press_position);

  editor.userMovesMouseTo(release_position);
  editor.userReleasesMouseAt(release_position);

  assert(diagram.nExistingNodes()==2);
}


int main()
{
  testDeletingANode();
  testDeletingAConnectedNode();
  testChangingText();
  testChangingText2();
  testChangingText3();
  testChangingText4();
  testChangingText5();
  testSettingDiagramPtr();
  testSettingDiagramPtrWithAnEmptyFocusedNode();
  testClickingOnBackgroundTwice();
  testEscapeWithAFocusedEmptyNode();
  testEscapeWithAFocusedNonEmptyNode();
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
  testClickingOnAFocusedNode1();
  testClickingOnAFocusedNode2();
  testClickingOnAFocusedNodeAboveFirstLine();
  testClickingOnAFocusedNodeBelowFirstLine();
  testCopyingANodeByCtrlDrag();

  ImportTester().runWithEmptyDiagram();
  ImportTester().runWithBadDiagram();
  ImportTester().runWithExistingNodes();
}
