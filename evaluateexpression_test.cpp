#include "evaluateexpression.hpp"

#include <vector>


using std::vector;


static void testListWithInvalidElement()
{
  int index = 0;
  Parser parser{"[,2]",index};
  vector<float> input_values;
  int input_index = 0;
  Optional<Any> maybe_result =
    evaluateExpression(parser,input_values,input_index);
  assert(!maybe_result);
}


static void testEmptyList()
{
  int index = 0;
  Parser parser{"[]",index};
  vector<float> input_values;
  int input_index = 0;
  Optional<Any> maybe_result =
    evaluateExpression(parser,input_values,input_index);
  assert(maybe_result);
  assert(maybe_result->asVector().empty());
}


int main()
{
  testListWithInvalidElement();
  testEmptyList();
}
