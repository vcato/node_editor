#include "diagrameditor.hpp"

#include <cassert>

using std::string;
using std::vector;


DiagramEditor::DiagramEditor(Diagram &diagram_arg)
: diagram_ptr(&diagram_arg)
{
}


void DiagramEditor::setDiagramPtr(Diagram *arg)
{
  if (node_editor.aNodeIsFocused()) {
    node_editor.unfocus();
  }
  diagram_ptr = arg;
  redraw();
}


void DiagramEditor::deleteNode(int index)
{
  diagram().deleteNode(index);
}


string &DiagramEditor::focusedText()
{
  return node_editor.focusedText(diagram());
}


void DiagramEditor::enterPressed()
{
  if (node_editor.focused_node_index>=0) {
    node_editor.text_editor.enter();
    diagram().removeInvalidInputs();
    redraw();
  }
}


bool DiagramEditor::aNodeIsFocused() const
{
  return node_editor.aNodeIsFocused();
}


void DiagramEditor::backspacePressed()
{
  if (node_editor.selected_node_index>=0) {
    deleteNode(node_editor.selected_node_index);
    node_editor.selected_node_index = -1;
    redraw();
    return;
  }

  if (node_editor.aNodeIsFocused()) {
    node_editor.text_editor.backspace();
    redraw();
    return;
  }
}


int DiagramEditor::addNode(const std::string &text,const Point2D &position)
{
  // The node editor keeps a pointer to a node, but Nodes may move in memory.
  assert(!node_editor.aNodeIsFocused());

  int node_index = diagram().addNode(text);
  diagram().node(node_index).header_text_object.position = position;
  return node_index;
}


void
  DiagramEditor::connectNodes(
    int output_node_index,
    int output_index,
    int input_node_index,
    int input_index
  )
{
  diagram().connectNodes(
    output_node_index,output_index,
    input_node_index,input_index
  );
}


void DiagramEditor::textTyped(const string &new_text)
{
  if (node_editor.aNodeIsFocused()) {
    node_editor.text_editor.textTyped(new_text);
    redraw();
    return;
  }
}


void DiagramEditor::unfocus()
{
  node_editor.unfocus();
  diagram().removeInvalidInputs();
}


Rect DiagramEditor::nodeRect(const TextObject &text_object) const
{
  return withMargin(rectAroundText(text_object),5);
}


Rect DiagramEditor::nodeHeaderRect(const TextObject &text_object) const
{
  if (text_object.text=="") {
    Point2D start = text_object.position;
    Point2D end = start;
    return Rect{start,end};
  }
  return nodeRect(text_object);
}


Point2D
  DiagramEditor::alignmentPoint(
    const Rect &rect,
    float horizontal_alignment,
    float vertical_alignment
  ) const
{
  float h = horizontal_alignment;
  float v = vertical_alignment;
  float x1 = rect.start.x;
  float x2 = rect.end.x;
  float y1 = rect.start.y;
  float y2 = rect.end.y;
  float x = x1*(1-h) + x2*h;
  float y = y1*(1-v) + y2*v;
  return Point2D(x,y);
}


TextObject
  DiagramEditor::alignedTextObject(
    const std::string &text,
    const Point2D &position,
    float horizontal_alignment,
    float vertical_alignment
  ) const
{
  TextObject text_object;
  text_object.text = text;
  text_object.position = Point2D(0,0);
  Rect rect = rectAroundText(text_object);
  Point2D offset =
    alignmentPoint(rect,horizontal_alignment,vertical_alignment);
  text_object.position = position - offset;
  return text_object;
}


Rect
  DiagramEditor::nodeBodyRect(
    const Node &node,
    const Rect &header_rect
  ) const
{
  // The top of the rectangle should be the bottom of the header text
  // object.
  float top_y = header_rect.start.y;

  // The left side should be the left of the header rect
  float left_x = header_rect.start.x;

  // Start with the bottom being at the top.
  // For each string, we determine its rectangle, and then move the bottom
  // to the bottom of that rectangle.
  float bottom_y = top_y;

  vector<string> strings = node.strings();

  for (const auto& s : strings) {
    Rect r =
      rectAroundText(alignedTextObject(
        s,
        Point2D(left_x,bottom_y),
        /*horizontal_alignment*/0,
        /*vertical_alignment*/1
      ));
    bottom_y = r.start.y;
  }

  // The right side is the maximum of all the right sides of the individual
  // text objects.
  float right_x = header_rect.end.x;

  for (const auto &s : strings) {
    Rect r =
      rectAroundText(
        alignedTextObject(
          s,
          Point2D(left_x,top_y),
          /*horizontal_alignment*/0,
          /*vertical_alignment*/1
        )
      );
    if (r.end.x > right_x) {
      right_x = r.end.x;
    }
  }

  Rect body_rect;

  body_rect.start.x = left_x;
  body_rect.start.y = bottom_y;
  body_rect.end.x = right_x;
  body_rect.end.y = top_y;

  return body_rect;
}


TextObject
  DiagramEditor::inputTextObject(const string &s,float left_x,float y) const
{
  TextObject t =
    alignedTextObject(
      s,
      Point2D(left_x,y),
      /*horizontal_alignment*/0,
      /*vertical_alignment*/1
    );
  return t;
}


NodeRenderInfo DiagramEditor::nodeRenderInfo(const Node &node) const
{
  const TextObject &header_text_object = node.header_text_object;

  // We need to determine a rectangle that fits around the contents.
  Rect header_rect = nodeHeaderRect(header_text_object);

  NodeRenderInfo render_info;
  Rect body_rect = nodeBodyRect(node,header_rect);
  render_info.header_rect = header_rect;

  float left_x = body_rect.start.x;
  float right_x = body_rect.end.x;

  float margin = 5;

  float left_outer_x = left_x - margin;
  float right_outer_x = right_x + margin;

  float top_y = body_rect.end.y;

  size_t n_lines = node.lines.size();

  vector<float> line_start_ys(n_lines);
  vector<float> line_end_ys(n_lines);

  {
    float y = top_y;

    for (size_t line_index=0; line_index!=n_lines; ++line_index) {
      const auto &line = node.lines[line_index];
      TextObject t = inputTextObject(line.text,left_x,y);
      render_info.text_objects.push_back(t);
      Rect r = rectAroundText(t);
      line_start_ys[line_index] = r.start.y;
      line_end_ys[line_index] = r.end.y;
      y = r.start.y;
    }
  }

  for (size_t line_index=0; line_index!=n_lines; ++line_index) {
    const auto &line = node.lines[line_index];
    float line_start_y = line_start_ys[line_index];
    float line_end_y = line_end_ys[line_index];
    float line_center_y = (line_start_y + line_end_y)/2;

    if (line.has_input) {
      float connector_x = (left_outer_x - connector_radius - 5);
      float connector_y = line_center_y;

      Circle c;
      c.center = Point2D(connector_x,connector_y);
      c.radius = connector_radius;
      render_info.input_connector_circles.push_back(c);
    }
  }

  for (
    size_t
      statement_index = 0,
      n_statements=node.statements.size(),
      line_index = 0;
    statement_index!=n_statements;
    ++statement_index
  ) {
    const auto &statement = node.statements[statement_index];
    const auto statement_n_lines = statement.n_lines;
    float expression_start_y =
      line_start_ys[line_index + statement_n_lines - 1];
    float expression_end_y = line_end_ys[line_index];
    float expression_center_y = (expression_start_y + expression_end_y)/2;

    if (statement.has_output) {
      float connector_x = (right_outer_x + connector_radius + 5);
      float connector_y = expression_center_y;

      Circle c;
      c.center = Point2D(connector_x,connector_y);
      c.radius = connector_radius;
      render_info.output_connector_circles.push_back(c);
    }
    line_index += statement_n_lines;
  }

  render_info.body_outer_rect = body_rect;
  render_info.body_outer_rect.start.x = left_outer_x;
  render_info.body_outer_rect.end.x = right_outer_x;

  return render_info;
}


int DiagramEditor::indexOfNodeContaining(const Point2D &p)
{
  for (NodeIndex i : diagram().existingNodeIndices()) {
    Node &node = diagram().node(i);
    NodeRenderInfo render_info = nodeRenderInfo(node);
    if (render_info.header_rect.contains(p)) {
      return i;
    }
    if (render_info.body_outer_rect.contains(p)) {
      return i;
    }
  }

  return -1;
}


Circle DiagramEditor::nodeInputCircle(const Node &node,int input_index)
{
  NodeRenderInfo render_info = nodeRenderInfo(node);
  return render_info.input_connector_circles[input_index];
}


bool
  DiagramEditor::nodeInputContains(
    int node_index,
    int input_index,
    const Point2D &p
  )
{
  return nodeInputCircle(node(node_index),input_index).contains(p);
}


Circle DiagramEditor::nodeOutputCircle(const Node &node,int output_index)
{
  NodeRenderInfo render_info = nodeRenderInfo(node);
  return render_info.output_connector_circles[output_index];
}


bool
  DiagramEditor::nodeOutputContains(
    int node_index,
    int output_index,
    const Point2D &p
  )
{
  return nodeOutputCircle(node(node_index),output_index).contains(p);
}


NodeConnectorIndex
  DiagramEditor::indexOfNodeConnectorContaining(const Point2D &p)
{
  for (NodeIndex i : diagram().existingNodeIndices()) {
    int n_inputs = node(i).nInputs();
    for (int j=0; j!=n_inputs; ++j) {
      if (nodeInputContains(i,j,p)) {
        NodeConnectorIndex index;
        index.node_index = i;
        index.input_index = j;
        index.output_index = -1;
        return index;
      }
    }
    int n_outputs = node(i).nOutputs();
    for (int j=0; j!=n_outputs; ++j) {
      if (nodeOutputContains(i,j,p)) {
        NodeConnectorIndex index;
        index.node_index = i;
        index.input_index = -1;
        index.output_index = j;
        return index;
      }
    }
  }

  return NodeConnectorIndex::null();
}


void DiagramEditor::mouseReleasedAt(Point2D mouse_release_position)
{
  if (!selected_node_connector_index.isNull()) {
    NodeConnectorIndex release_index =
      indexOfNodeConnectorContaining(temp_source_pos);
    if (!release_index.isNull()) {
      if (selected_node_connector_index.input_index>=0 &&
          release_index.output_index>=0) {
        // Connected an input to an output
        int input_node_index = selected_node_connector_index.node_index;
        int input_index = selected_node_connector_index.input_index;
        int output_node_index = release_index.node_index;
        int output_index = release_index.output_index;
        connectNodes(
          output_node_index,output_index,
          input_node_index,input_index
        );
      }
      else if (selected_node_connector_index.output_index>=0 &&
               release_index.input_index>=0) {
        // Connected an output to an input
        int input_node_index = release_index.node_index;
        int input_index = release_index.input_index;
        int output_node_index = selected_node_connector_index.node_index;
        int output_index = selected_node_connector_index.output_index;
        connectNodes(
          output_node_index,output_index,
          input_node_index,input_index
        );
      }
    }
    selected_node_connector_index.clear();
    redraw();
    return;
  }

  if (mouse_press_position==mouse_release_position) {
    if (node_editor.node_was_selected) {
      node_editor.focusNode(node_editor.selected_node_index,diagram());
      node_editor.selected_node_index = -1;
      redraw();
      return;
    }
  }
}


void DiagramEditor::mousePressedAt(Point2D p)
{
  mouse_press_position = p;
  node_editor.node_was_selected = false;

  if (node_editor.aNodeIsFocused()) {
    NodeIndex focused_node_index = node_editor.focused_node_index;
    Node &focused_node = node_editor.focusedNode(diagram());
    unfocus();
    if (focused_node.isEmpty()) {
      deleteNode(focused_node_index);
    }
  }

  {
    int i = indexOfNodeContaining(p);

    if (i>=0) {
      node_editor.node_was_selected = (i==node_editor.selected_node_index);
      node_editor.selected_node_index = i;
      node_editor.focused_node_index = -1;
      original_node_position = node(i).header_text_object.position;
      redraw();
      return;
    }
  }

  node_editor.selected_node_index = -1;
  selected_node_connector_index = NodeConnectorIndex::null();

  {
    NodeConnectorIndex i = indexOfNodeConnectorContaining(p);

    if (i!=NodeConnectorIndex::null()) {
      selected_node_connector_index = i;
      temp_source_pos = mouse_press_position;
      redraw();
      return;
    }
  }

  int new_node_index = addNode("",mouse_press_position);
  node_editor.focusNode(new_node_index,diagram());

  redraw();
}
