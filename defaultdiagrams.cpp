#include "defaultdiagrams.hpp"

#include <sstream>
#include "diagramio.hpp"


using std::istringstream;


static Diagram makeDiagram(const char *text)
{
  istringstream stream(text);
  Diagram diagram;
  scanDiagramFrom(stream,diagram);
  return diagram;
}


Diagram posExprDiagram()
{
  const char *text = R"text(
diagram {
  node {
    id: 4
    position: [485,153]
    text {
      "$-$"
    }
    connection {
      input_index: 0
      source_node_id: 5
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 7
      source_output_index: 0
    }
  }
  node {
    id: 5
    position: [266,183]
    text {
      "global_position"
    }
  }
  node {
    id: 7
    position: [274,108]
    text {
      "$.globalVec($)"
    }
    connection {
      input_index: 0
      source_node_id: 8
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 8
      source_output_index: 1
    }
  }
  node {
    id: 8
    position: [59,158]
    text {
      "target_body"
      "local_position"
    }
  }
  node {
    id: 10
    position: [559,233]
    text {
      "$.pos=$"
    }
    connection {
      input_index: 0
      source_node_id: 13
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 4
      source_output_index: 0
    }
  }
  node {
    id: 13
    position: [283,233]
    text {
      "$"
    }
    connection {
      input_index: 0
      source_node_id: 8
      source_output_index: 0
    }
  }
}
)text";

  return makeDiagram(text);
}


Diagram fromComponentsDiagram()
{
  const char *text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position: [71,280]\n"
    "    text {\n"
    "      \"x\"\n"
    "      \"y\"\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 2\n"
    "    position: [183,280]\n"
    "    text {\n"
    "      \"[$,$]\"\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 1\n"
    "      source_output_index: 0\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 1\n"
    "      source_node_id: 1\n"
    "      source_output_index: 1\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 4\n"
    "    position: [299,264]\n"
    "    text {\n"
    "      \"return $\"\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 2\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "}\n";

  return makeDiagram(text);
}


Diagram fromBodyDiagram()
{
  const char *text = R"text(
diagram {
  node {
    id: 1
    position: [263,328]
    text {
      "$.pos($)"
    }
    connection {
      input_index: 0
      source_node_id: 5
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 5
      source_output_index: 1
    }
  }
  node {
    id: 4
    position: [410,313]
    text {
      "return $"
    }
    connection {
      input_index: 0
      source_node_id: 1
      source_output_index: 0
    }
  }
  node {
    id: 5
    position: [50,328]
    text {
      "source_body"
      "local_position"
    }
  }
}
)text";

  return makeDiagram(text);
}




Diagram localPositionDiagram()
{
  const char *text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position: [100,180]\n"
    "    text {\n"
    "      \"[$,$]\"\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 2\n"
    "      source_output_index: 0\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 1\n"
    "      source_node_id: 3\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 2\n"
    "    position: [20,187]\n"
    "    text {\n"
    "      \"x\"\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 3\n"
    "    position: [20,135]\n"
    "    text {\n"
    "      \"y\"\n"
    "    }\n"
    "  }\n"
    "  node {\n"
    "    id: 4\n"
    "    position: [224,162]\n"
    "    text {\n"
    "      \"local_position=$\"\n"
    "    }\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 1\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "}\n";

  return makeDiagram(text);
}
