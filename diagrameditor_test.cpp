#include "diagrameditor.hpp"

#include <cassert>


using std::string;


namespace {
struct FakeDiagramEditor : DiagramEditor {
  int redraw_count = 0;

  int userAddsANodeWithText(const string &text)
  {
    TextObject new_text_object;
    new_text_object.position = Point2D{0,0};
    new_text_object.text = text;
    int node_index = addNode(new_text_object);
    return node_index;
  }

  void userFocusesNode(int node_index)
  {
    focused_node_index = node_index;
  }

  void userPressesEnter()
  {
    enterPressed();
  }

  bool nodeIsSelected(int node_index)
  {
    return node_index==selected_node1_index;
  }

  bool aNodeIsFocused() const
  {
    return focused_node_index>=0;
  }

  void setFocusedNode(int node_index)
  {
    focused_node_index = node_index;
  }

  int nodeInputCount(int node_index) const
  {
    return node1s[node_index].inputs.size();
  }

  virtual void redraw()
  {
    ++redraw_count;
  }
};
}


static void test1()
{
  FakeDiagramEditor editor;

  // User clicks on the background.
  // A single node exists.
  // The node is focused.
}


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


int main()
{
  test1();
  test2();
  test3();
}
