#include "statementtext.hpp"

#include <cassert>

using std::vector;


static void testWithEmptyString()
{
  vector<int> result = statementLineCounts("");
  assert(result.empty());
}


static void testWithOneLine()
{
  vector<int> result = statementLineCounts("1\n");
  assert(result.size()==1);
  assert(result[0]==1);
}


static void testWithTwoLines()
{
  vector<int> result = statementLineCounts("1\n2\n");
  vector<int> expected_result{1,1};
  assert(result==expected_result);
}


static void testWithVectorSpanningTwoLines()
{
  {
    vector<int> result = statementLineCounts("[1,2,\n3]\n");
    vector<int> expected_result{2};
    assert(result==expected_result);
  }
  {
    vector<int> result = statementLineCounts("[x,y,z\n]\n");
    vector<int> expected_result{2};
    assert(result==expected_result);
  }
}


static void testWithIncompleteVectorSpanningTwoLines()
{
  {
    vector<int> result = statementLineCounts("[x,y,z\n");
    vector<int> expected_result{1};
    assert(result==expected_result);
  }
}


int main()
{
  testWithEmptyString();
  testWithOneLine();
  testWithTwoLines();
  testWithVectorSpanningTwoLines();
  testWithIncompleteVectorSpanningTwoLines();
}
