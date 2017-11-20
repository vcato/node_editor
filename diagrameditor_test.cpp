#include "diagrameditor.hpp"

#include <cassert>


using std::string;


namespace {
struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;

  FakeDiagramEditor(Diagram &diagram) : DiagramEditor(diagram) { }

#if USE_NODE1
  int userAddsANodeWithText(const string &text)
  {
    TextObject new_text_object;
    new_text_object.position = Point2D{0,0};
    new_text_object.text = text;
    int node_index = addNode(new_text_object);
    return node_index;
  }
#endif

  int userAddsANode2WithText(const string &text)
  {
    return addNode2(text,Point2D(0,0));
  }

  void userSelectsNode2(int node_index)
  {
    node2_editor.selectNode(node_index);
  }

  void userFocusesNode2(int node_index)
  {
    node2_editor.focusNode(node_index,node2s());
  }

  void
    userConnects(
      int input_node_index,
      int input_index,
      int output_node_index,
      int output_index
    )
  {
    connectNodes(
      input_node_index,input_index,output_node_index,output_index
    );
  }

#if USE_NODE1
  void userFocusesNode(int node_index)
  {
    node1_editor.focused_node_index = node_index;
  }
#endif

  void userPressesEnter()
  {
    enterPressed();
  }

  void userPressesBackspace()
  {
    backspacePressed();
  }

#if USE_NODE1
  bool nodeIsSelected(int node_index)
  {
    return node_index==node1_editor.selected_node_index;
  }
#endif

#if USE_NODE1
  bool aNodeIsFocused() const
  {
    return node1_editor.focused_node_index>=0;
  }
#endif

#if USE_NODE1
  void setFocusedNode(int node_index)
  {
    node1_editor.focused_node_index = node_index;
  }
#endif

#if USE_NODE1
  int nodeInputCount(int node_index) const
  {
    return node1s[node_index].inputs.size();
  }
#endif

  virtual void redraw()
  {
    ++redraw_count;
  }
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


#if USE_NODE1
static void test2()
{
  // If a node has text and is focused, then when the user presses enter,
  // the node should become unfocused and selected.

  FakeDiagramEditor editor;

  int node_index = editor.userAddsANodeWithText("test");
  editor.userFocusesNode(node_index);

  editor.redraw_count = 0;
  editor.userPressesEnter();

  assert(editor.nodeIsSelected(node_index));
  assert(!editor.aNodeIsFocused());
  assert(editor.redraw_count==1);
}
#endif


#if USE_NODE1
static void test3()
{
  // If a node has text "+", then when the user presses enter, the node
  // should get two inputs.

  FakeDiagramEditor editor;

  int node_index = editor.userAddsANodeWithText("+");
  editor.setFocusedNode(node_index);
  assert(editor.nodeInputCount(node_index)==0);
  editor.userPressesEnter();
  assert(editor.nodeInputCount(node_index)==2);
}
#endif


static void testDeletingANode()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int node_index = editor.userAddsANode2WithText("test");
  editor.userSelectsNode2(node_index);
  editor.userPressesBackspace();
  assert(editor.nNode2s()==0);
}


static void testChangingText()
{
  Diagram diagram;
  FakeDiagramEditor editor(diagram);
  int n1 = editor.userAddsANode2WithText("5");
  int n2 = editor.userAddsANode2WithText("show($)");
  editor.userConnects(n2,0,n1,0);
  editor.userFocusesNode2(n1);
  editor.userPressesBackspace();
  assert(diagram.node(n2).inputs[0].source_node_index<0);
}


int main()
{
  test1();
#if USE_NODE1
  test2();
  test3();
#endif
  testDeletingANode();
  testChangingText();
}
