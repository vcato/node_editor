#include "diagramio.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include "streamparser.hpp"
#include "stringutil.hpp"
#include "wrapperstate.hpp"
#include "optional.hpp"
#include "diagramwrapperstate.hpp"


using Node = DiagramNode;
using std::ostream;
using std::istream;
using std::string;
using std::istringstream;
using std::cerr;


void printDiagramOn(ostream &stream,const Diagram &diagram)
{
  printStateOn(stream,makeDiagramWrapperState(diagram));
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
  int first_char = stream.get();

  if (first_char != '"') {
    assert(false);
  }

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
struct DiagramParser : StreamParser {
  DiagramParser(istream &stream_arg) : StreamParser(stream_arg) { }

  void scanNodeConnectionSection(Node &node)
  {
    // Scan input_index
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

    // Scan source_node_id
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

    scanEndOfLine();

    // Scan source_output_index
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

    scanEndOfLine();

    // Scan close brace
    scanWord();

    if (word!="}") {
      scanWord();
      cerr << "word: " << word << "\n";
      assert(false);
    }

    scanEndOfLine();
  }

  void scanNodeTextSection(Node &node)
  {
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

  Optional<Point2D> maybeScanPoint2DSection()
  {
    Optional<int> maybe_x;
    Optional<int> maybe_y;

    bool was_successful =
      scanSection(
        /*line_handler*/[&](const string &tag){
          if (tag=="x:") {
            maybe_x = nextIntFrom(stream);
            scanEndOfLine();
          }
          else if (tag=="y:") {
            maybe_y = nextIntFrom(stream);
            scanEndOfLine();
          }
          else {
            cerr << "tag: " << tag << "\n";
            assert(false);
          }

          return true;
        },
        /*section_handler*/[&](const string &/*tag*/){
          assert(false);
          ignoreSection();
          return true;
        }
      );

    if (!was_successful) {
      return {};
    }

    if (!maybe_x) {
      setError("Missing x");
      return {};
    }

    if (!maybe_y) {
      setError("Missing y");
      return {};
    }

    return Point2D(*maybe_x,*maybe_y);
  }

  bool scanNodeSection(Diagram &diagram)
  {
    scanWord();

    if (word!="id:") {
      // The id is required to be first so that we can create the node.
      assert(false);
      return false;
    }

    int id = nextIntFrom(stream);
    Node &node = diagram.createNode(id-1);
    scanEndOfLine();

    return
      scanSection(
        /*line_handler*/[&](const string &tag){
          if (tag=="position:") {
            node.setPosition(DiagramCoords(nextPoint2DFrom(stream)));
          }
          else if (tag=="line:") {
            skipWhitespace();
            string value = nextDoubleQuotedStringFrom(stream);
            node.lines.push_back(Node::Line(value));
          }

          scanEndOfLine();
        },
        /*section_handler*/[&](const string &tag){
          if (tag=="connection") {
            scanNodeConnectionSection(node);
            return true;
          }

          if (tag=="text") {
            scanNodeTextSection(node);
            return true;
          }

         if (tag=="position") {
           Optional<Point2D> maybe_position = maybeScanPoint2DSection();
           if (!maybe_position) {
             return false;
           }
           node.setPosition(DiagramCoords(*maybe_position));
         }

          return true;
        }
      );
  }

  template <typename LineHandler,typename SectionHandler>
  bool
    scanSection(
      const LineHandler &line_handler,
      const SectionHandler &section_handler
    )
  {
    for (;;) {
      scanWord();

      if (word=="}") {
        scanEndOfLine();
        return true;
      }

      if (endsWith(word,":")) {
        line_handler(word);
      }
      else {
        string tag = word;

        if (!scanOpenBrace()) {
          return false;
        }

        scanEndOfLine();
        if (!section_handler(tag)) {
          return false;
        }
      }
    }
  }

  bool scanOpenBrace()
  {
    skipWhitespace();

    if (stream.peek()=='\n' || stream.peek()==EOF) {
      setError("Expected '{'");
      return false;
    }

    scanWord();

    if (word!="{") {
      setError("Expected '{'");
      return false;
    }

    return true;
  }

  bool scanDiagram(Diagram &diagram)
  {
    scanWord();

    if (word!="diagram") {
      setError("Expected tag 'diagram'");
      return false;
    }

    if (!scanOpenBrace()) {
      return false;
    }

    scanEndOfLine();

    bool section_was_scanned_successfully =
      scanSection(
         /*line_handler*/[&](const string &/*tag*/){
           scanEndOfLine();
         },
         /*section_handler*/[&](const string &tag){
           if (tag=="node") {
             return scanNodeSection(diagram);
           }

           return ignoreSection();
         }
      );

    if (!section_was_scanned_successfully) {
      return false;
    }

    for (auto i : diagram.existingNodeIndices()) {
      diagram.node(i).updateInputsAndOutputs();
    }

    return true;
  }

  bool ignoreSection()
  {
    return scanSection(
      /*line_handler*/[&](const string &/*tag*/){ return scanEndOfLine(); },
      /*section_handler*/[&](const string &/*tag*/){ return ignoreSection(); }
    );
  }
};
}


void scanDiagramFrom(std::istream &stream,Diagram &diagram,string &error)
{
  DiagramParser parser(stream);

  if (!parser.scanDiagram(diagram)) {
    std::ostringstream error_stream;
    error_stream << "error on line " << parser.line_number << ": " <<
      parser.error;
    error = error_stream.str();
  }
}
