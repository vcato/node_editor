#include "node.hpp"

#include <cassert>
#include <sstream>


using std::vector;
using std::string;
using std::istringstream;
using std::cerr;


void Node::removeLine(int line_index)
{
  lines.erase(lines.begin() + line_index);
}


void Node::updateInputsAndOutputs()
{
  for (auto &line : lines) {
    line.has_input = lineTextHasInput(line.text);
    line.has_output = lineTextHasOutput(line.text);
  }

  updateNInputs();
  updateNOutputs();
}


void Node::addInputsAndOutputs()
{
  for (auto &line : lines) {
    if (!line.has_input) {
      line.has_input = lineTextHasInput(line.text);
    }
    if (!line.has_output) {
      line.has_output = lineTextHasOutput(line.text);
    }
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
  size_t n_outputs = 0;

  for (auto &line : node.lines) {
    if (line.has_output) {
      ++n_outputs;
    }
  }

  return n_outputs;
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


void Node::setText(const std::string &text)
{
  Node &node = *this;
  if (text=="") {
    node.lines.clear();
    node.lines.resize(1,Node::Line(""));
    node.updateInputsAndOutputs();
    assert(node.nLines()==1);
    assert(!node.lines[0].has_output);
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
