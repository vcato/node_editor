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
    node_editor.focusNode(node_index,diagram);
  }

  void userUnfocusesNode()
  {
    unfocus();
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
  editor.userConnects(n2,0,n1,0);
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
  editor.userConnects(n2,0,n1,0);
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
  assert(diagram.node(n1).lines[0].has_output);
  editor.userPressesBackspace();

  // Once a line gets an output, we want it to keep the output until
  // we move to another line to avoid disconnections due to temporary
  // text changes.
  assert(diagram.node(n1).lines[0].has_output);

  editor.userTypesText("x=");

  assert(diagram.node(n1).lines[0].has_output);
}


int main()
{
  test1();
  testDeletingANode();
  testDeletingAConnectedNode();
  testChangingText();
  testChangingText2();
}
