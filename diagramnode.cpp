#include "diagramnode.hpp"

#include <cassert>
#include "statementtext.hpp"
#include "linetext.hpp"
#include "splitlines.hpp"

using std::vector;
using std::string;
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


void Node::addInputs()
{
  for (auto &line : lines) {
    int new_n_inputs = lineTextInputCount(line.text);

    if (line.n_inputs<new_n_inputs) {
      line.n_inputs = new_n_inputs;
    }
  }

  updateNInputs();
}


void Node::addOutputs()
{
  size_t n_statements = statements.size();
  vector<bool> statement_output_flags = determineStatementOutputFlags();

  for (size_t i=0; i!=n_statements; ++i) {
    if (!statements[i].has_output) {
      statements[i].has_output = statement_output_flags[i];
    }
  }

  assert(!statements.empty());

  updateNOutputs();
}


void Node::addInputsAndOutputs()
{
  addInputs();
  addOutputs();
}


std::vector<std::string> Node::lineTexts() const
{
  std::vector<std::string> result;

  for (const auto &line : lines) {
    result.push_back(line.text);
  }

  return result;
}


string Node::text() const
{
  string result;

  for (auto &line_text : lineTexts()) {
    result += line_text + '\n';
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


void Node::setPosition(const DiagramPoint &arg)
{
  header_text_object.position = arg;
}


void Node::setText(const std::string &text)
{
  Node &node = *this;

  node.lines.clear();

  if (text=="") {
    node.lines.resize(1,Node::Line(""));
    node.updateInputsAndOutputs();
    assert(node.nLines()==1);
    assert(!node.statements[0].has_output);
    assert(node.nOutputs()==0);
    return;
  }

  vector<string> lines = splitLines(text);

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


void Node::breakLine(const TextPosition &position)
{
  Node &node = *this;
  const int line_index = position.line_index;
  const int column_index = position.column_index;

  {
    std::string &text = lines[line_index].text;

    node.lines.insert(
      node.lines.begin() + line_index + 1,
      Node::Line(text.substr(column_index))
    );
  }

  {
    std::string &text = node.lines[line_index].text;
    node.lines[line_index].text.erase(
      text.begin() + column_index,
      text.end()
    );
  }

  node.updateInputsAndOutputs();
}


void Node::joinLines(const TextPosition &position)
{
  Node &node = *this;
  const int line_index = position.line_index;
  node.lines[line_index].text += node.lines[line_index+1].text;
  node.removeLine(line_index+1);
  node.updateInputsAndOutputs();
}


int Node::inputIndexAt(const TextPosition &position) const
{
  int input_index = 0;

  for (int i=0; i!=position.line_index; ++i) {
    input_index += lines[i].n_inputs;
  }

  for (int j=0; j!=position.column_index; ++j) {
    if (lines[position.line_index].text[j]=='$') {
      ++input_index;
    }
  }

  return input_index;
}


void Node::deleteCharacter(const TextPosition &position)
{
  std::string &text = lines[position.line_index].text;
  const int column_index = position.column_index;

  if (text[column_index] == '$') {
    int input_index = inputIndexAt(position);

    if (inputs[input_index].source_node_index != nullNodeIndex()) {
      Input old_input = inputs[input_index];
      inputs.erase(inputs.begin() + input_index);
      inputs.push_back(old_input);
    }
    else {
      inputs.erase(inputs.begin() + input_index);
    }

    assert(lines[position.line_index].n_inputs>0);
    --lines[position.line_index].n_inputs;
  }

  text.erase(text.begin() + column_index);
}


void
  Node::insertCharacter(
    const TextPosition &position,
    char c
  )
{
  const int column_index = position.column_index;
  std::string &text = lines[position.line_index].text;

  if (c=='$') {
    int input_index = inputIndexAt(position);

    // Find the first unused input and move it to the new position.
    int n_used_inputs = countInputs(*this);
    int n_inputs = inputs.size();

    if (n_inputs > n_used_inputs) {
      Input new_input = inputs[n_used_inputs];
      inputs.erase(inputs.begin()  + n_used_inputs);
      inputs.insert(inputs.begin() + input_index,new_input);
    }
    else {
      inputs.insert(inputs.begin() + input_index,Input());
    }

    ++lines[position.line_index].n_inputs;
  }

  text.insert(text.begin() + column_index,c);
  addOutputs();
}
