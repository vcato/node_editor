#include "diagramio.hpp"

#include <cassert>


using Node = DiagramNode;


void printDiagramOn(std::ostream &stream,const Diagram &diagram)
{
  stream << "diagram {\n";

  if (diagram.nExistingNodes()==0) {
  }
  else {
    for (auto i : diagram.existingNodeIndices()) {
      const Node &node = diagram.node(i);
      stream << "  node {\n";
      stream << "    id: " << i+1 << "\n";
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
  }

  stream << "}\n";
}
