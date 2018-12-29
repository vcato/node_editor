#include "defaultdiagrams.hpp"

#include <sstream>
#include "diagramio.hpp"
#include "makediagram.hpp"



using std::istringstream;


const Diagram& posExprDiagram()
{
  const char *text = R"text(
diagram {
  node {
    id: 4
    position {
      x: 485
      y: 153
    }
    line: "$-$"
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
    position {
      x: 253
      y: 176
    }
    line: "global_position"
  }
  node {
    id: 7
    position {
      x: 274
      y: 108
    }
    line: "$.globalVec($)"
    connection {
      input_index: 0
      source_node_id: 8
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 14
      source_output_index: 0
    }
  }
  node {
    id: 8
    position {
      x: 33
      y: 222
    }
    line: "target_body"
  }
  node {
    id: 14
    position {
      x: 39
      y: 71
    }
    line: "local_position"
  }
  node {
    id: 15
    position {
      x: 579
      y: 222
    }
    line: "PosExpr(body=$,pos=$)"
    connection {
      input_index: 0
      source_node_id: 8
      source_output_index: 0
    }
    connection {
      input_index: 1
      source_node_id: 4
      source_output_index: 0
    }
  }
  node {
    id: 16
    position {
      x: 929
      y: 203
    }
    line: "return $"
    connection {
      input_index: 0
      source_node_id: 15
      source_output_index: 0
    }
  }
}
)text";

  static Diagram result = makeDiagram(text);
  return result;
}


const Diagram& fromComponentsDiagram()
{
  const char *text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position {\n"
    "      x: 71\n"
    "      y: 280\n"
    "    }\n"
    "    line: \"x\"\n"
    "    line: \"y\"\n"
    "  }\n"
    "  node {\n"
    "    id: 2\n"
    "    position {\n"
    "      x: 183\n"
    "      y: 280\n"
    "    }\n"
    "    line: \"[$,$]\"\n"
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
    "    position {\n"
    "      x: 299\n"
    "      y: 264\n"
    "    }\n"
    "    line: \"return $\"\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 2\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "}\n";

  static Diagram result = makeDiagram(text);
  return result;
}


const Diagram& fromBodyDiagram()
{
  const char *text = R"text(
diagram {
  node {
    id: 1
    position {
      x: 263
      y: 328
    }
    line: "$.pos($)"
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
    position {
      x: 410
      y: 313
    }
    line: "return $"
    connection {
      input_index: 0
      source_node_id: 1
      source_output_index: 0
    }
  }
  node {
    id: 5
    position {
      x: 50
      y: 328
    }
    line: "source_body"
    line: "local_position"
  }
}
)text";

  static Diagram result = makeDiagram(text);
  return result;
}




const Diagram &localPositionDiagram()
{
  const char *text =
    "diagram {\n"
    "  node {\n"
    "    id: 1\n"
    "    position {\n"
    "      x: 100\n"
    "      y: 180\n"
    "    }\n"
    "    line: \"[$,$]\"\n"
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
    "    position {\n"
    "      x: 20\n"
    "      y: 187\n"
    "    }\n"
    "    line: \"x\"\n"
    "  }\n"
    "  node {\n"
    "    id: 3\n"
    "    position {\n"
    "      x: 20\n"
    "      y: 135\n"
    "    }\n"
    "    line: \"y\"\n"
    "  }\n"
    "  node {\n"
    "    id: 4\n"
    "    position {\n"
    "      x: 224\n"
    "      y: 162\n"
    "    }\n"
    "    line: \"return $\"\n"
    "    connection {\n"
    "      input_index: 0\n"
    "      source_node_id: 1\n"
    "      source_output_index: 0\n"
    "    }\n"
    "  }\n"
    "}\n";

  static Diagram result = makeDiagram(text);
  return result;
}
