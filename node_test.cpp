#include "node.hpp"

#include <cassert>


int main()
{
  Node node;
  node.setText("");
  assert(node.isEmpty());
}
