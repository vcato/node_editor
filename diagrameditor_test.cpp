#include "diagrameditor.hpp"

#include <cassert>


using std::string;


namespace {
struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;

  FakeDiagramEditor(Diagram &diagram) : DiagramEditor(diagram) { }

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

  void userPressesEnter()
  {
    enterPressed();
  }

  void userPressesBackspace()
  {
    backspacePressed();
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
  testDeletingANode();
  testChangingText();
}
