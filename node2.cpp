#include "node2.hpp"

#include <cassert>
#include <sstream>


using std::vector;
using std::string;
using std::istringstream;


void Node2::removeLine(int line_index)
{
  lines.erase(lines.begin() + line_index);
}


void Node2::updateInputsAndOutputs()
{
  for (auto &line : lines) {
    line.has_input = lineTextHasInput(line.text);
    line.has_output = lineTextHasOutput(line.text);
  }

  updateNInputs();
  updateNOutputs();
}


std::vector<std::string> Node2::strings() const
{
  std::vector<std::string> result;

  for (const auto &line : lines) {
    result.push_back(line.text);
  }

  return result;
}


void Node2::setNInputs(size_t arg)
{
  inputs.resize(arg);
}


void Node2::setNOutputs(size_t arg)
{
  n_outputs = arg;
}


void Node2::updateNInputs()
{
  setNInputs(countInputs(*this));
}


void Node2::updateNOutputs()
{
  setNOutputs(countOutputs(*this));
}


size_t Node2::countInputs(const Node2 &node)
{
  size_t n_inputs = 0;

  for (auto &line : node.lines) {
    if (line.has_input) {
      ++n_inputs;
    }
  }

  return n_inputs;
}


size_t Node2::countOutputs(const Node2 &node)
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


void Node2::setText(const std::string &text)
{
  Node2 &node = *this;
  if (text=="") {
    node.lines.resize(1,Node2::Line(""));
    node.updateInputsAndOutputs();
    return;
  }

  vector<string> lines = split(text);

  for (const auto &line : lines) {
    node.lines.push_back(Node2::Line(line));
  }

  if (endsWith(text,"\n")) {
    node.lines.push_back(Node2::Line(""));
  }

  node.updateInputsAndOutputs();
}


bool Node2::isEmpty() const
{
  if (lines.size()==1 && lines[0].text=="") return true;
  return false;
}
