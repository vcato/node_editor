#include "wrapperstate.hpp"

#include <algorithm>
#include <sstream>
#include "optional.hpp"
#include "streamparser.hpp"
#include "stringutil.hpp"
#include "printindent.hpp"



using std::istringstream;
using std::string;
using std::ostream;
using std::cerr;


namespace {
struct WrapperValuePrinter {
  ostream &stream;

  void operator()(WrapperValue::Void) const
  {
  }

  void operator()(int arg) const
  {
    stream << ": " << arg;
  }

  void operator()(const string &arg) const
  {
    stream << ": " << quoted(arg);
  }

  void operator()(const WrapperValue::Enumeration &arg) const
  {
    stream << ": " << arg.name;
  }
};
}


void printStateOn(ostream &stream,const WrapperState &state,int indent)
{
  printIndent(stream,indent);

  stream << state.tag;
  state.value.visit(WrapperValuePrinter{stream});

  if (state.children.empty() && !state.value.isVoid()) {
    stream << "\n";
  }
  else {
    stream << " {\n";
    int n_children = state.children.size();

    for (int i=0; i!=n_children; ++i) {
      printStateOn(stream,state.children[i],indent+1);
    }

    printIndent(stream,indent);
    stream << "}\n";
  }
}


static Optional<WrapperValue> scanValue(StreamParser &parser)
{
  parser.skipWhitespace();

  if (parser.stream.peek()=='"') {
    parser.stream.get();
    string value;

    for (;;) {
      int c = parser.stream.get();

      if (c=='\n' ) {
        assert(false);
      }

      if (c==EOF) {
        assert(false);
      }

      if (c=='"') {
        return WrapperValue(value);
      }

      value.push_back(c);
    }
  }

  parser.scanWord();

  if (!parser.stream) {
    assert(false);
  }

  if (startsWith(parser.word,'"')) {
    assert(false);
  }

  if (isdigit(parser.word[0])) {
    NumericValue value;
    istringstream value_stream(parser.word);
    value_stream >> value;

    if (!value_stream) {
      assert(false);
    }

    return WrapperValue(value);
  }

  return WrapperValue(WrapperValue::Enumeration{parser.word});
}


static Optional<WrapperState> scanState(StreamParser &parser);


static void scanChildren(WrapperState &state,StreamParser &parser)
{
  parser.scanWord();

  if (parser.word!="{") {
    parser.error = "Unexpected "+parser.word;
    return;
  }

  parser.scanEndOfLine();

  for (;;) {
    parser.beginLine();

    parser.scanWord();

    if (parser.word=="}") {
      parser.scanEndOfLine();
      break;
    }

    Optional<WrapperState> maybe_child_result = scanState(parser);

    if (!maybe_child_result) {
      return;
    }

    state.children.push_back(*maybe_child_result);
  }
}


static Optional<WrapperState> scanState(StreamParser &parser)
{
  if (endsWith(parser.word,":")) {
    string tag = withoutRight(parser.word,1);
    WrapperState state(tag);
    Optional<WrapperValue> maybe_value = scanValue(parser);

    if (!maybe_value) {
      assert(false);
    }

    state.value = *maybe_value;

    parser.skipWhitespace();

    if (parser.stream.peek()=='{') {
      scanChildren(state,parser);

      if (parser.hadError()) {
        assert(false);
      }
    }

    parser.scanEndOfLine();

    return state;
  }

  string tag = parser.word;
  WrapperState state(tag);

  if (parser.stream.peek()=='\n') {
    return state;
  }

  scanChildren(state,parser);

  if (parser.hadError()) {
    return {};
  }

  return state;
}


ScanStateResult scanStateFrom(std::istream & stream)
{
  StreamParser parser(stream);

  parser.scanWord();

  if (!parser.stream) {
    assert(false);
  }

  Optional<WrapperState> maybe_state = scanState(parser);

  if (!maybe_state) {
    return ScanStateResult::error(parser.error);
  }

  return *maybe_state;
}
