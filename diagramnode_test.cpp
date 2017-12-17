#include "diagramnode.hpp"

#include <cassert>
#include <iostream>


using Node = DiagramNode;
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
  assert(node.expressions.size()==1);
}


int main()
{
  testIsEmpty();
  testMultiLineExpression();
}
