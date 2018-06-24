#include "scenewrapper.hpp"

#include <sstream>
#include "streamvector.hpp"
#include "wrapperutil.hpp"
#include "basicvariant.hpp"

using std::ostringstream;
using std::ostream;
using std::string;
using std::cerr;
using std::vector;


namespace {
struct WrapperValuePolicy {
  struct NoInitTag {};
  struct Void {};
  struct Enumeration { EnumerationWrapper::Value index; };

  WrapperValuePolicy()
  : WrapperValuePolicy(Void{})
  {
  }

  WrapperValuePolicy(Void)
  : _type(void_type)
  {
    new (&_value.void_value)Void{};
  }

  WrapperValuePolicy(NumericWrapper::Value arg)
  : _type(numeric_type)
  {
    new (&_value.numeric_value)auto(arg);
  }

  WrapperValuePolicy(StringWrapper::Value arg)
  : _type(string_type)
  {
    new (&_value.string_value)auto(arg);
  }

  WrapperValuePolicy(Enumeration arg)
  : _type(enumeration_type)
  {
    new (&_value.enumeration_value)auto(arg);
  }

  enum Type {
    void_type,
    numeric_type,
    enumeration_type,
    string_type
  };

  protected:
    WrapperValuePolicy(NoInitTag)
    {
    }

    union Value {
      Value() {}
      ~Value() {}

      Void void_value;
      NumericWrapper::Value numeric_value;
      EnumerationWrapper::Value enumeration_value;
      std::string string_value;
    };

    template <typename V>
    static auto withMemberPtrFor(Type t,const V& v)
    {
      switch (t) {
        case void_type:  return v(&Value::void_value);
        case numeric_type: return v(&Value::numeric_value);
        case enumeration_type: return v(&Value::enumeration_value);
        case string_type: return v(&Value::string_value);
      }

      assert(false);
    }

    Type _type;
    Value _value;
};
}


using WrapperValue = BasicVariant<WrapperValuePolicy>;

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
      value = WrapperValue::Enumeration{enumeration_wrapper.value()};
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


namespace {
struct WrapperState {
  string label;
  WrapperValue value;
  vector<WrapperState> children;
};
}


static WrapperState stateOf(const Wrapper &wrapper)
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


namespace {
struct WrapperValuePrinter {
  ostream &stream;

  void operator()(WrapperValue::Void) const
  {
  }

  void operator()(int arg) const
  {
    stream << " " << arg;
  }

  void operator()(const string &arg) const
  {
    stream << " " << quoted(arg);
  }
};
}


static void
  printState(ostream &stream,const WrapperState &state,int indent = 0)
{
  for (int i=0; i!=indent; ++i) {
    stream << "  ";
  }

  stream << state.label << ":";
  state.value.visit(WrapperValuePrinter{stream});
  stream << "\n";
  int n_children = state.children.size();

  for (int i=0; i!=n_children; ++i) {
    printState(stream,state.children[i],indent+1);
  }
}


static void printTree(ostream &stream,const Wrapper &wrapper,int indent = 0)
{
  printState(stream,stateOf(wrapper),indent);
}


static SceneWrapper::SceneObserver unusedObserver()
{
  return
    SceneWrapper::SceneObserver(
      [](const Wrapper::TreeObserver &){
        assert(false);
      }
    );
}


static void testHierarchy()
{
  Scene scene;
  Scene::Body &body = scene.addBody();
  scene.addChildBodyTo(body);
  SceneWrapper wrapper(scene,unusedObserver(),"Scene");
  ostringstream stream;
  printTree(stream,wrapper);
  string output = stream.str();

  auto expected_output =
    "Scene:\n"
    "  Body:\n"
    "    name: \"Body1\"\n"
    "    position:\n"
    "      x: 0\n"
    "      y: 0\n"
    "    Body:\n"
    "      name: \"Body2\"\n"
    "      position:\n"
    "        x: 0\n"
    "        y: 0\n";

  if (output!=expected_output) {
    cerr << "output:\n";
    cerr << output << "\n";
  }

  assert(output==expected_output);
}


static void
  addBody2(const Wrapper &wrapper,const TreePath &path,ostream &stream)
{
  struct TreeObserver : Wrapper::TreeObserver {
    ostream &stream;

    TreeObserver(ostream &stream_arg)
    : stream(stream_arg)
    {
    }

    virtual void itemAdded(const TreePath &path)
    {
      stream << "addItem: path=" << path << "\n";
    }

    virtual void itemReplaced(const TreePath &)
    {
      assert(false);
    }

    virtual void enumarationValuesChanged(const TreePath &) const
    {
      assert(false);
    }

    virtual void itemRemoved(const TreePath &)
    {
      assert(false);
    }
  };

  TreeObserver operation_handler(stream);

  executeAddBodyFunction(wrapper,path,operation_handler);
}


static void
  addBodyTo(const Wrapper &wrapper,const TreePath &path,ostream &stream)
{
  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &body_wrapper){
      addBody2(body_wrapper,path,stream);
    }
  );
}


static void testAddingBodies()
{
  Scene scene;
  SceneWrapper::SceneObserver notify([](const Wrapper::TreeObserver &){});
  SceneWrapper wrapper(scene,notify,"Scene");
  ostringstream stream;

  int body_index = 2;
  addBodyTo(wrapper,{},stream);
  addBodyTo(wrapper,{0},stream);
  addBodyTo(wrapper,{0,body_index},stream);

  assert(scene.nBodies()==1);
  assert(scene.bodies()[0].nChildren()==1);
  assert(scene.bodies()[0].children[0].nChildren()==1);

  string body_index_str = std::to_string(body_index);
  string commands = stream.str();
  string expected_commands =
    "addItem: path=[0]\n"
    "addItem: path=[0," + body_index_str + "]\n"
    "addItem: path=[0," + body_index_str + "," + body_index_str + "]\n";


  if (commands!=expected_commands) {
    cerr << "commands:\n";
    cerr << commands << "\n";
    cerr << "expected_commands:\n";
    cerr << expected_commands << "\n";
  }

  assert(commands==expected_commands);
}


static void testGettingState()
{
  Scene scene;
  SceneWrapper::SceneObserver notify(
    [](const Wrapper::TreeObserver &){ assert(false); }
  );
  Scene::Body &body = scene.addBody();
  scene.addChildBodyTo(body);
  SceneWrapper wrapper(scene,notify,"Scene");
  WrapperState state = stateOf(wrapper);
  ostringstream stream;
  printState(stream,state);
  string output = stream.str();

  auto expected_output =
    "Scene:\n"
    "  Body:\n"
    "    name: \"Body1\"\n"
    "    position:\n"
    "      x: 0\n"
    "      y: 0\n"
    "    Body:\n"
    "      name: \"Body2\"\n"
    "      position:\n"
    "        x: 0\n"
    "        y: 0\n";

  if (output!=expected_output) {
    cerr << "output:\n";
    cerr << output << "\n";
  }

  assert(output==expected_output);
}


#if 0
static void testBuildingFromState()
{
  Scene scene;
  SceneWrapper wrapper(scene,unusedObserver(),"Scene");
  WrapperState state;
  state.label = "Scene";
  wrapper.setState(state);
  assert(stateOf(wrapper)==state);
}
#endif


int main()
{
  testHierarchy();
  testAddingBodies();
  testGettingState();
  // testBuildingFromState();
}
