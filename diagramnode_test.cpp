#include "diagramnode.hpp"

#include <cassert>


using Node = DiagramNode;



int main()
{
  Node node;
  node.setText("");
  assert(node.isEmpty());
}
