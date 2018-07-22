#include "wrapperstate.hpp"

#include <algorithm>
#include "optional.hpp"
#include "streamparser.hpp"
#include "stringutil.hpp"


using std::string;
using std::ostream;
using std::cerr;


static string quoted(const string &s)
{
  return '"' + s + '"';
}


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


static void printIndent(ostream &stream,int indent)
{
  for (int i=0; i!=indent; ++i) {
    stream << "  ";
  }
}


void printStateOn(ostream &stream,const WrapperState &state,int indent)
{
  printIndent(stream,indent);

  stream << state.tag;
  state.value.visit(WrapperValuePrinter{stream});

  if (state.children.empty()) {
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
  parser.scanWord();

  if (!parser.stream) {
    assert(false);
  }

  if (startsWith(parser.word,'"')) {
    assert(false);
  }

  return WrapperValue(WrapperValue::Enumeration{parser.word});
}


static Optional<WrapperState> scanState(StreamParser &parser)
{
  parser.scanWord();

  if (!parser.stream) {
    assert(false);
  }

  if (endsWith(parser.word,":")) {
    string tag = withoutRight(parser.word,1);
    WrapperState state(tag);
#if 1
    Optional<WrapperValue> maybe_value = scanValue(parser);

    if (!maybe_value) {
      assert(false);
    }

    state.value = *maybe_value;
#else
    assert(false);
#endif
    return state;
  }

  string tag = parser.word;
  WrapperState state(tag);
  parser.scanWord();

  if (parser.word!="{") {
    assert(false);
  }

  for (;;) {
    parser.scanWord();

    if (parser.word=="}") {
      assert(false);
      break;
    }

    Optional<WrapperState> maybe_child_result = scanState(parser);

    if (!maybe_child_result) {
      assert(false);
    }

    state.children.push_back(*maybe_child_result);
  }
}


ScanStateResult scanStateFrom(std::istream & stream)
{
  StreamParser parser(stream);

  Optional<WrapperState> maybe_state = scanState(parser);

  if (!maybe_state) {
    return ScanStateResult::error(parser.error);
  }

  return *maybe_state;
}
