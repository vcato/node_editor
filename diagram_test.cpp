#include "diagram.hpp"

#include <cassert>
#include <sstream>

using std::ostringstream;
using std::string;
using std::cerr;


int main()
{
  {
    Diagram diagram;
    int node_index =  diagram.addNode("5");
    diagram.evaluate();
    assert(diagram.node(node_index).outputs[0].value==5);
  }
  {
    Diagram diagram;
    int node_index =  diagram.addNode("6");
    diagram.evaluate();
    assert(diagram.node(node_index).outputs[0].value==6);
  }
  {
    Diagram diagram;
    diagram.addNode("show(5)");
    ostringstream stream;
    diagram.evaluate(stream);
    string output = stream.str();
    assert(output=="5\n");
  }
  {
    Diagram diagram;
    diagram.addNode("");
    ostringstream stream;
    diagram.evaluate(stream);
  }
  {
    Diagram diagram;
    diagram.addNode("show($)");
    ostringstream stream;
    diagram.evaluate(stream);
  }
  {
    Diagram diagram;
    int n1 = diagram.addNode("5");
    int n2 = diagram.addNode("show($)");
    int input_node_index = n2;
    int input_index = 0;
    int output_node_index = n1;
    int output_index = 0;
    diagram.connectNodes(
      input_node_index,input_index,output_node_index,output_index
    );
    ostringstream stream;
    diagram.evaluate(stream);
    string output = stream.str();
    assert(output=="5\n");
  }
  {
    Diagram diagram;
    int n1 = diagram.addNode("5");
    int n2 = diagram.addNode("show($)");
    int input_node_index = n2;
    int input_index = 0;
    int output_node_index = n1;
    int output_index = 0;
    diagram.connectNodes(
      input_node_index,input_index,output_node_index,output_index
    );
    diagram.setNodeText(n1,"");
    assert(diagram.node(n1).nOutputs()==0);
    assert(diagram.node(n2).inputs[0].source_node_index<0);
    ostringstream stream;
    // diagram.evaluate(stream);
  }
}
