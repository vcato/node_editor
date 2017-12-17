#include "diagramnode.hpp"

#include <cassert>
#include <sstream>


using std::vector;
using std::string;
using std::istringstream;
using std::cerr;

using Node = DiagramNode;


void Node::removeLine(int line_index)
{
  lines.erase(lines.begin() + line_index);
}


void Node::updateInputsAndOutputs()
{
  size_t n_lines = lines.size();
  expressions.resize(n_lines);

  for (size_t i=0; i!=n_lines; ++i) {
    auto& line = lines[i];
    line.has_input = lineTextHasInput(line.text);
    expressions[i].n_lines = 1;
    expressions[i].has_output = lineTextHasOutput(line.text);
  }

  updateNInputs();
  updateNOutputs();
}


string Node::joinLines(int start,int n_lines)
{
  if (n_lines==1) {
    return lines[start].text;
  }

  cerr << "start: " << start << "\n";
  cerr << "n_lines: " << n_lines << "\n";
  assert(false);
}


void Node::addInputsAndOutputs()
{
  for (auto &line : lines) {
    if (!line.has_input) {
      line.has_input = lineTextHasInput(line.text);
    }
  }

  {
    size_t line_index = 0;
    for (auto &expression : expressions) {
      auto n_lines = expression.n_lines;
      if (!expression.has_output) {
        expression.has_output =
          lineTextHasOutput(joinLines(line_index,n_lines));
      }
      line_index += n_lines;
    }
    assert(line_index==lines.size());
  }

  updateNInputs();
  updateNOutputs();
}


std::vector<std::string> Node::strings() const
{
  std::vector<std::string> result;

  for (const auto &line : lines) {
    result.push_back(line.text);
  }

  return result;
}


void Node::setNInputs(size_t arg)
{
  inputs.resize(arg);
}


void Node::setNOutputs(size_t arg)
{
  outputs.resize(arg);
}


void Node::updateNInputs()
{
  setNInputs(countInputs(*this));
}


void Node::updateNOutputs()
{
  setNOutputs(countOutputs(*this));
}


size_t Node::countInputs(const Node &node)
{
  size_t n_inputs = 0;

  for (auto &line : node.lines) {
    if (line.has_input) {
      ++n_inputs;
    }
  }

  return n_inputs;
}


size_t Node::countOutputs(const Node &node)
{
  size_t n_expression_outputs = 0;

  for (auto &expression : node.expressions) {
    if (expression.has_output) {
      ++n_expression_outputs;
    }
  }

  return n_expression_outputs;
}


static vector<string> split(const string &text)
{
  istringstream stream(text);
  vector<string> result;
  string line;
  while (getline(stream,line)) {
    result.push_back(line);
  }
  return result;
}


void Node::setPosition(const Point2D &arg)
{
  header_text_object.position = arg;
}


void Node::setText(const std::string &text)
{
  Node &node = *this;
  if (text=="") {
    node.lines.clear();
    node.lines.resize(1,Node::Line(""));
    node.updateInputsAndOutputs();
    assert(node.nLines()==1);
    assert(!node.expressions[0].has_output);
    assert(node.nOutputs()==0);
    return;
  }

  vector<string> lines = split(text);

  for (const auto &line : lines) {
    node.lines.push_back(Node::Line(line));
  }

  if (endsWith(text,"\n")) {
    node.lines.push_back(Node::Line(""));
  }

  node.updateInputsAndOutputs();
}


bool Node::isEmpty() const
{
  if (lines.size()==1 && lines[0].text=="") return true;
  return false;
}
