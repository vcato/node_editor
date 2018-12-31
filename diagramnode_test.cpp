#include "diagramnode.hpp"

#include <cassert>
#include <iostream>

using Node = DiagramNode;
using TextPosition = Node::TextPosition;
using std::cerr;


static void testIsEmpty()
{
  Node node;
  node.setText("");
  assert(node.isEmpty());
}


static void testMultiLineExpression()
{
  Node node;
  node.setText("[x,y,z\n]");
  assert(node.statements.size()==1);
}


static void testSwappingInputs()
{
  Node node;
  node.setText("$+$");
  assert(node.inputs.size()==2);
  node.inputs[0].source_node_index = 0;
  node.inputs[1].source_node_index = 1;

  node.deleteCharacter(TextPosition{0,0});
  assert(node.inputs[0].source_node_index==1);
  assert(node.inputs[1].source_node_index==0);

  node.deleteCharacter(TextPosition{0,0});

  assert(node.inputs.size()==2);
  assert(node.inputs[0].source_node_index==1);
  assert(node.inputs[1].source_node_index==0);

  node.insertCharacter(TextPosition{0,1},'+');

  assert(node.inputs.size()==2);
  assert(node.inputs[0].source_node_index==1);
  assert(node.inputs[1].source_node_index==0);

  node.insertCharacter(TextPosition{0,2},'$');

  assert(node.inputs[0].source_node_index==1);
  assert(node.inputs[1].source_node_index==0);
}


int main()
{
  testIsEmpty();
  testMultiLineExpression();
  testSwappingInputs();
}
