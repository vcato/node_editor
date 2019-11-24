#include "splitlines.hpp"

#include <sstream>

using std::vector;
using std::string;
using std::istringstream;


vector<string> splitLines(const string &text)
{
  istringstream stream(text);
  vector<string> result;
  string line;

  while (getline(stream,line)) {
    result.push_back(line);
  }

  return result;
}
