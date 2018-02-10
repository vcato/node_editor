#include "scenewrapper.hpp"

#include <sstream>


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


int main()
{
  Scene scene;
  auto notify = [](){ assert(false); };
  scene.addBody();
  scene.bodies()[0].position.x = 1;
  SceneWrapper wrapper(scene,notify);
  ostringstream stream;
  printTree(stream,wrapper);
  string output = stream.str();

  auto expected_output =
    "Scene\n"
    "  Body\n"
    "    position\n"
    "      x\n"
    "      y\n";

  if (output!=expected_output) {
    cerr << "output:\n";
    cerr << output << "\n";
  }

  assert(output==expected_output);
}
