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
  stream << "  node {\n";
  stream << "    id: " << i+1 << "\n";
  stream << "    position: [" << node.position().x << "," << node.position().y << "]\n";
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

  if (diagram.nExistingNodes()==0) {
  }
  else {
    for (auto i : diagram.existingNodeIndices()) {
      const Node &node = diagram.node(i);
      printNodeOn(stream,node,i);
    }
  }

  stream << "}\n";
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

  void scanTextBody(Node &node)
  {
    for (;;) {
      for (;;) {
        int c = stream.get();
        if (c==EOF) {
          assert(false);
          break;
        }
        if (c=='\n') {
          assert(false);
          break;
        }
        if (c=='"') {
          break;
        }
        if (c=='}') {
          scanEndOfLine();
          return;
        }
      }
      string value;
      for (;;) {
        int c = stream.get();
        if (c=='"') {
          break;
        }
        value.push_back(c);
      }
      scanEndOfLine();
      node.lines.push_back(Node::Line(value));
    }
  }


  void scanConnectionBody(Node &node)
  {
    scanWord();

    if (word=="input_index:") {
      int input_index = 0;
      stream >> input_index;
      scanEndOfLine();
      int n_inputs = node.inputs.size();
      if (input_index>=n_inputs) {
        node.inputs.resize(n_inputs+1);
      }
      Node::Input &input = node.inputs[input_index];
      scanWord();
      if (word=="source_node_id:") {
        int source_node_id;
        stream >> source_node_id;
        if (source_node_id<1) {
          assert(false);
        }
        input.source_node_index = source_node_id-1;
      }
      scanWord();
      if (word=="source_output_index:") {
        int source_output_index;
        stream >> source_output_index;
        if (source_output_index<0) {
          assert(false);
        }
        input.source_output_index = source_output_index;
      }
      scanWord();
      if (word=="}") {
      }
      else {
        scanWord();
        cerr << "word: " << word << "\n";
        assert(false);
      }
    }
  }

  void scanDiagramBody(Diagram &diagram)
  {
    for (;;) {
      scanWord();
      if (word=="}") {
        return;
      }
      if (word=="node") {
        scanWord();
        if (word=="{") {
          scanEndOfLine();
          scanWord();
          if (word=="id:") {
            int id;
            stream >> id;
            Node &node = diagram.createNode(id-1);
            scanEndOfLine();
            scanWord();
            if (word=="position:") {
              string position_string;
              stream >> position_string;
              if (position_string=="[0,0]") {
                node.setPosition(Point2D(0,0));
              }
              else {
                istringstream position_stream(position_string);
                if (position_stream.peek()!='[') {
                  assert(false);
                }
                position_stream.get();
                float x;
                position_stream >> x;
                if (position_stream.peek()!=',') {
                  assert(false);
                }
                position_stream.get();
                float y;
                position_stream >> y;
                node.setPosition({x,y});
              }
            }
            scanWord();
            if (word=="text") {
              scanWord();
              if (word=="{") {
                scanEndOfLine();
                scanTextBody(node);
              }
              else {
                assert(false);
              }
            }
            for (;;) {
              scanWord();
              if (word=="}") {
                break;
              }
              else {
                if (word=="connection") {
                  scanWord();
                  if (word=="{") {
                    scanEndOfLine();
                    scanConnectionBody(node);
                  }
                }
              }
            }
          }
        }
        else {
          assert(false);
        }
      }
      else {
        cerr << "word: " << word << "\n";
        assert(false);
      }
    }
  }
};
}


void scanDiagramFrom(std::istream &stream,Diagram &diagram)
{
  Parser parser(stream);
  parser.scanWord();
  string &word = parser.word;

  if (word=="diagram") {
    parser.scanWord();

    if (word=="{") {
      parser.scanEndOfLine();
      parser.scanDiagramBody(diagram);
    }
    else {
      assert(false);
    }

    for (auto i : diagram.existingNodeIndices()) {
      diagram.node(i).updateInputsAndOutputs();
    }
  }
  else {
    assert(false);
  }
}
