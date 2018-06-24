#include "wrapperstate.hpp"

#include <algorithm>


using std::string;
using std::ostream;
using std::cerr;


static WrapperValue valueOf(const Wrapper &wrapper)
{
  WrapperValue value;

  struct Visitor : Wrapper::Visitor {
    WrapperValue &value;

    Visitor(WrapperValue &value_arg)
    : value(value_arg)
    {
    }

    void operator()(const VoidWrapper &) const override
    {
      value = WrapperValue::Void{};
    }

    void operator()(const NumericWrapper &numeric_wrapper) const override
    {
      value = numeric_wrapper.value();
    }

    void
      operator()(const EnumerationWrapper &enumeration_wrapper) const override
    {
      const auto &names = enumeration_wrapper.enumerationNames();
      const auto index = enumeration_wrapper.value();
      value = WrapperValue::Enumeration{names[index]};
    }

    void operator()(const StringWrapper &string_wrapper) const override
    {
      value = string_wrapper.value();
    }
  };

  Visitor visitor(value);

  wrapper.accept(visitor);

  return value;
}


WrapperState stateOf(const Wrapper &wrapper)
{
  WrapperState result;
  result.label = wrapper.label();
  result.value = valueOf(wrapper);

  for (int i=0, n=wrapper.nChildren(); i!=n; ++i) {
    wrapper.withChildWrapper(
      i,
      [&](const Wrapper &child){
        result.children.push_back(stateOf(child));
      }
    );
  }

  return result;
}


static string quoted(const string &s)
{
  return '"' + s + '"';
}


static string makeTag(string label)
{
  std::transform(label.begin(),label.end(),label.begin(),::tolower);
  std::replace(label.begin(),label.end(),' ','_');
  return label;
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
    stream << ": " << makeTag(arg.name);
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

  stream << makeTag(state.label);
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
