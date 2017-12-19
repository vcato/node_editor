#include "diagramnode.hpp"

#include <cassert>
#include <sstream>
#include "statementtext.hpp"


using std::vector;
using std::string;
using std::istringstream;
using std::cerr;

using Node = DiagramNode;


void Node::removeLine(int line_index)
{
  lines.erase(lines.begin() + line_index);
}


vector<bool> Node::determineStatementOutputFlags() const
{
  size_t line_index = 0;
  size_t n_statements = statements.size();
  vector<bool> statement_output_flags(n_statements,false);

  for (size_t i=0; i!=n_statements; ++i) {
    auto n_lines = statements[i].n_lines;
    auto expression_text = joinLines(line_index,n_lines,' ');
    statement_output_flags[i] = lineTextHasOutput(expression_text);
    line_index += n_lines;
  }

  return statement_output_flags;
}


void Node::updateInputsAndOutputs()
{
  {
    size_t n_lines = lines.size();

    for (size_t i=0; i!=n_lines; ++i) {
      auto& line = lines[i];
      line.n_inputs = lineTextInputCount(line.text);
    }
  }

  auto full_text = joinLines(0,lines.size(),'\n');
  vector<int> line_counts = statementLineCounts(full_text);
  size_t n_statements = line_counts.size();
  statements.resize(n_statements);

  for (size_t i=0; i!=n_statements; ++i) {
    auto n_lines = line_counts[i];
    statements[i].n_lines = n_lines;
  }

  vector<bool> statement_output_flags = determineStatementOutputFlags();

  for (size_t i=0; i!=n_statements; ++i) {
    statements[i].has_output = statement_output_flags[i];
  }

  updateNInputs();
  updateNOutputs();
}


string Node::joinLines(int start,int n_lines,char separator) const
{
  if (n_lines==1) {
    return lines[start].text + separator;
  }

  string result("");

  for (int i=0; i!=n_lines; ++i) {
    result += lines[i].text + separator;
  }

  return result;
}


void Node::addInputsAndOutputs()
{
  for (auto &line : lines) {
    int new_n_inputs = lineTextInputCount(line.text);

    if (line.n_inputs<new_n_inputs) {
      line.n_inputs = new_n_inputs;
    }
  }

  size_t n_statements = statements.size();
  vector<bool> statement_output_flags = determineStatementOutputFlags();

  for (size_t i=0; i!=n_statements; ++i) {
    if (!statements[i].has_output) {
      statements[i].has_output = statement_output_flags[i];
    }
  }

  assert(!statements.empty());

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
    n_inputs += line.n_inputs;
  }

  return n_inputs;
}


size_t Node::countOutputs(const Node &node)
{
  size_t n_expression_outputs = 0;

  for (auto &statement : node.statements) {
    if (statement.has_output) {
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
    assert(!node.statements[0].has_output);
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
