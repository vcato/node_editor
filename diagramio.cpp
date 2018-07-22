#include "diagramio.hpp"

#include <cassert>
#include <sstream>


using Node = DiagramNode;
using std::ostream;
using std::istream;
using std::string;
using std::istringstream;
using std::cerr;


static void printNodeOn(ostream &stream,const Node &node,int i)
{
  const Point2D &pos = node.position();
  int id = i+1;

  stream << "  node {\n";
  stream << "    id: " << id << "\n";
  stream << "    position: [" << pos.x << "," << pos.y << "]\n";
  stream << "    text {\n";

  for (auto &line : node.lines) {
    stream << "      \"" << line.text << "\"\n";
  }

  stream << "    }\n";
  int n_inputs = node.inputs.size();

  for (int input_index=0; input_index!=n_inputs; ++input_index) {
    const auto &input = node.inputs[input_index];
    NodeIndex source_node_index = input.source_node_index;
    int source_output_index = input.source_output_index;

    if (source_node_index!=nullNodeIndex()) {
      stream << "    connection {\n";
      stream << "      input_index: " << input_index << "\n";
      stream << "      source_node_id: " << source_node_index + 1 << "\n";
      stream << "      source_output_index: "
        << source_output_index << "\n";
      stream << "    }\n";
    }
  }

  stream << "  }\n";
}


void printDiagramOn(ostream &stream,const Diagram &diagram)
{
  stream << "diagram {\n";

  for (auto i : diagram.existingNodeIndices()) {
    const Node &node = diagram.node(i);
    printNodeOn(stream,node,i);
  }

  stream << "}\n";
}


static float nextFloatFrom(istream &stream)
{
  float x;
  stream >> x;

  if (!stream) {
    assert(false);
  }

  return x;
}


static int nextIntFrom(istream &stream)
{
  int value = 0;
  stream >> value;

  if (!stream) {
    assert(false); // not implemented
  }

  return value;
}


static string nextStringFrom(istream &stream)
{
  string position_string;
  stream >> position_string;

  if (!stream) {
    assert(false); // not implemented
  }

  return position_string;
}


static void skipCharFrom(istream &stream,char c)
{
  if (stream.peek()!=c) {
    assert(false);
  }

  stream.get();
}


static Point2D nextPoint2DFrom(istream &stream)
{
  string position_string = nextStringFrom(stream);
  istringstream position_stream(position_string);
  skipCharFrom(position_stream,'[');
  float x = nextFloatFrom(position_stream);
  skipCharFrom(position_stream,',');
  float y = nextFloatFrom(position_stream);
  skipCharFrom(position_stream,']');
  return {x,y};
}


static string nextDoubleQuotedStringFrom(istream &stream)
{
  if (stream.peek()!='"') {
    assert(false);
  }

  stream.get();

  string value;

  for (;;) {
    int c = stream.get();

    if (c=='"') {
      break;
    }

    value.push_back(c);
  }

  return value;
}


namespace {
struct Parser {
  istream &stream;
  string word;

  Parser(istream &stream_arg) : stream(stream_arg) { }

  void scanWord()
  {
    stream >> word;
  }

  void scanEndOfLine()
  {
    for (;;) {
      int c = stream.get();
      if (c==EOF || c=='\n') break;
    }
  }

  void skipWhitespace()
  {
    while (stream.peek()==' ') {
      stream.get();
    }
  }
};
}


namespace {
struct DiagramParser : Parser {
  DiagramParser(istream &stream_arg) : Parser(stream_arg) { }

  void scanNodeConnection(Node &node)
  {
    scanWord();

    if (word!="{") {
      assert(false);
    }

    scanEndOfLine();
    scanWord();

    if (word!="input_index:") {
      assert(false);
    }

    int input_index = nextIntFrom(stream);
    scanEndOfLine();
    int n_inputs = node.inputs.size();

    if (input_index>=n_inputs) {
      node.inputs.resize(n_inputs+1);
    }

    Node::Input &input = node.inputs[input_index];
    scanWord();

    if (word!="source_node_id:") {
      assert(false);
    }

    {
      int source_node_id = nextIntFrom(stream);

      if (source_node_id<1) {
        assert(false);
      }

      input.source_node_index = source_node_id-1;
    }

    scanWord();

    if (word!="source_output_index:") {
      assert(false);
    }

    {
      int source_output_index = nextIntFrom(stream);

      if (source_output_index<0) {
        assert(false);
      }

      input.source_output_index = source_output_index;
    }

    scanWord();

    if (word!="}") {
      scanWord();
      cerr << "word: " << word << "\n";
      assert(false);
    }
  }

  void scanNodeText(Node &node)
  {
    scanWord();

    if (word!="{") {
      assert(false);
    }

    scanEndOfLine();

    for (;;) {
      skipWhitespace();

      if (stream.peek()=='}') {
        stream.get();
        scanEndOfLine();
        break;
      }

      string value = nextDoubleQuotedStringFrom(stream);
      scanEndOfLine();
      node.lines.push_back(Node::Line(value));
    }
  }

  void scanNode(Diagram &diagram)
  {
    scanWord();

    if (word!="{") {
      assert(false);
    }

    scanEndOfLine();
    scanWord();

    if (word!="id:") {
      // The id is required to be first so that we can create the node.
      assert(false);
    }

    int id = nextIntFrom(stream);
    Node &node = diagram.createNode(id-1);
    scanEndOfLine();

    for (;;) {
      scanWord();

      if (word=="}") {
        break;
      }

      if (word=="connection") {
        scanNodeConnection(node);
      }
      else if (word=="text") {
        scanNodeText(node);
      }
      else if (word=="position:") {
        node.setPosition(nextPoint2DFrom(stream));
      }
      else {
        assert(false);
      }
    }
  }

  void scanDiagram(Diagram &diagram)
  {
    scanWord();

    if (word!="diagram") {
      assert(false);
    }

    scanWord();

    if (word!="{") {
      assert(false);
    }

    scanEndOfLine();

    for (;;) {
      scanWord();

      if (word=="}") {
        break;
      }

      if (word=="node") {
        scanNode(diagram);
      }
      else {
        cerr << "word: " << word << "\n";
        assert(false);
      }
    }

    for (auto i : diagram.existingNodeIndices()) {
      diagram.node(i).updateInputsAndOutputs();
    }
  }
};
}


void scanDiagramFrom(std::istream &stream,Diagram &diagram)
{
  DiagramParser parser(stream);
  parser.scanDiagram(diagram);
}
