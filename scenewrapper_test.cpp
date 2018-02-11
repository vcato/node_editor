#include "scenewrapper.hpp"

#include <sstream>
#include "streamvector.hpp"


using std::ostringstream;
using std::ostream;
using std::string;
using std::cerr;


static void printTree(ostream &stream,const Wrapper &wrapper,int indent = 0)
{
  for (int i=0; i!=indent; ++i) {
    stream << "  ";
  }

  string label = wrapper.label();
  stream << label << "\n";
  int n_children = wrapper.nChildren();

  for (int i=0; i!=n_children; ++i) {
    wrapper.withChildWrapper(
      i,
      [&](const Wrapper &child){printTree(stream,child,indent+1);}
    );
  }
}


static void testHierarchy()
{
  Scene scene;
  auto notify = [](){ assert(false); };
  scene.addBody();
  scene.bodies()[0].position.x = 1;
  scene.bodies()[0].addChild();
  SceneWrapper wrapper(scene,notify);
  ostringstream stream;
  printTree(stream,wrapper);
  string output = stream.str();

  auto expected_output =
    "Scene\n"
    "  Body\n"
    "    name\n"
    "    position\n"
    "      x\n"
    "      y\n"
    "    Body\n"
    "      name\n"
    "      position\n"
    "        x\n"
    "        y\n";

  if (output!=expected_output) {
    cerr << "output:\n";
    cerr << output << "\n";
  }

  assert(output==expected_output);
}


static Wrapper::PerformOperationFunction
  findAddBodyFunction(const Wrapper &wrapper,const TreePath &path)
{
  Wrapper::PerformOperationFunction add_body_function;

  Wrapper::OperationVisitor visitor =
    [&](
      const Wrapper::OperationName &operation_name,
      Wrapper::PerformOperationFunction perform_operation_function
    ) {
      if (operation_name=="Add Body") {
      	add_body_function = perform_operation_function;
      }
    };

  wrapper.visitOperations(path,visitor);
  assert(add_body_function);
  return add_body_function;

}


static void
  addBody(const Wrapper &wrapper,const TreePath &path,ostream &stream)
{
  struct OperationHandler : Wrapper::OperationHandler {
    ostream &stream;

    OperationHandler(ostream &stream_arg)
    : stream(stream_arg)
    {
    }

    virtual void addItem(const TreePath &path)
    {
      stream << "addItem: path=" << path << "\n";
    }

    virtual void replaceTreeItems(const TreePath &)
    {
      assert(false);
    }
  };

  OperationHandler operation_handler(stream);

  Wrapper::PerformOperationFunction add_body_function =
    findAddBodyFunction(wrapper,path);

  add_body_function(operation_handler);
}


static void
  addBodyTo(const Wrapper &wrapper,const TreePath &path,ostream &stream)
{
  wrapper.visitWrapper(
    path,
    [&](const Wrapper &body_wrapper){
      addBody(body_wrapper,path,stream);
    }
  );
}


static void testAddingBodies()
{
  Scene scene;
  SceneWrapper wrapper(scene,[](){});
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
    "addItem: path=0\n"
    "addItem: path=0," + body_index_str + "\n"
    "addItem: path=0," + body_index_str + "," + body_index_str + "\n";


  if (commands!=expected_commands) {
    cerr << "commands:\n";
    cerr << commands << "\n";
    cerr << "expected_commands:\n";
    cerr << expected_commands << "\n";
  }

  assert(commands==expected_commands);
}


int main()
{
  testHierarchy();
  testAddingBodies();
}
