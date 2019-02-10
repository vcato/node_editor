#include "diagrameditor.hpp"

#include <cassert>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "ostreamvector.hpp"
#include "diagramio.hpp"

using std::string;
using std::vector;
using std::cerr;
using std::ifstream;
using std::ofstream;
using Node = DiagramNode;


template <typename Tag>
inline TaggedRect<Tag> withMargin(const TaggedRect<Tag> &rect,float margin)
{
  auto offset = Vector2D{margin,margin};
  return TaggedRect<Tag>{rect.start-offset,rect.end+offset};
}


DiagramEditor::DiagramEditor()
{
}


void DiagramEditor::setDiagramPtr(Diagram *arg)
{
  clearFocus();
  clearSelection();
  diagram_ptr2 = arg;
  redraw();
}


void DiagramEditor::setDiagramState(const DiagramState &arg)
{
  diagram_state = arg;

  redraw();
}


void DiagramEditor::setDiagramObserver(DiagramObserverPtr arg)
{
  // When we use a DiagramObserverPtr, the DiagramEditor becomes responsible
  // for handling the logic of updating the diagram state
  // if the observed diagram's state changes.
  // This function will update the diagram and the diagram state the same
  // way a user of the public API would, so that we isolate this logic.

  diagram_observer_ptr = std::move(arg);

  if (diagram_observer_ptr) {
    diagram_observer_ptr->diagram_state_changed_callback =
      [this]{
        setDiagramState(diagram_observer_ptr->diagramState());
      };
  }

  if (diagram_observer_ptr) {
    setDiagramPtr(&diagram_observer_ptr->diagram());
    setDiagramState(diagram_observer_ptr->diagramState());
  }
  else {
    setDiagramPtr(nullptr);
    setDiagramState(DiagramState());
  }
}


Diagram* DiagramEditor::diagramPtr() const
{
  return diagram_ptr2;
}


void DiagramEditor::deleteNode(int index)
{
  diagram().deleteNode(index);
}


string &DiagramEditor::focusedText()
{
  Diagram &diagram = this->diagram();
  assert(focused_node_index>=0);
  Node &node = focusedNode(diagram);
  return text_editor.focusedText(node);
}


void DiagramEditor::enterPressed()
{
  if (focused_node_index>=0) {
    text_editor.enter();
    diagram().removeInvalidInputs();
    notifyDiagramChanged();
    redraw();
  }
}


void DiagramEditor::notifyDiagramChanged()
{
  if (diagram_observer_ptr) {
    diagram_observer_ptr->notifyObservedDiagramThatDiagramChanged();
  }
}


void DiagramEditor::backspacePressed()
{
  if (aNodeIsSelected()) {
    for (NodeIndex i : selected_node_indices) {
      deleteNode(i);
    }

    clearSelection();
    notifyDiagramChanged();
    redraw();
    return;
  }

  if (aNodeIsFocused()) {
    text_editor.backspace();
    diagram().removeInvalidInputs();
    notifyDiagramChanged();
    redraw();
    return;
  }
}


void DiagramEditor::deletePressed()
{
  if (aNodeIsFocused()) {
    text_editor.deletePressed();
    notifyDiagramChanged();
    redraw();
    return;
  }
}


void DiagramEditor::escapePressed()
{
  if (!aNodeIsFocused()) return;

  NodeIndex previously_focused_node = focused_node_index;
  clearFocus();
  selectNode(previously_focused_node);

  redraw();
}


int
  DiagramEditor::addNode(
    const std::string &text,
    const DiagramCoords &position
  )
{
  // The node editor keeps a pointer to a node, but Nodes may move in memory.
  assert(!aNodeIsFocused());

  int node_index = diagram().createNodeWithText(text);
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
  if (aNodeIsFocused()) {
    text_editor.textTyped(new_text);
    notifyDiagramChanged();
    redraw();
    return;
  }
}


void DiagramEditor::unfocus()
{
  text_editor.endEditing();
  focused_node_index = noNodeIndex();
  diagram().removeInvalidInputs();
}


ViewportTextObject
  DiagramEditor::viewportTextObject(
    const DiagramTextObject &diagram_text_object
  ) const
{
  ViewportTextObject result;

  result.position =
    viewportCoordsFromDiagramCoords(diagram_text_object.position);
  result.text = diagram_text_object.text;

  return result;
}


ViewportRect
  DiagramEditor::nodeRect(const DiagramTextObject &text_object) const
{
  return withMargin(rectAroundText(viewportTextObject(text_object)),5);
}


ViewportRect
  DiagramEditor::nodeHeaderRect(const DiagramTextObject &text_object) const
{
  if (text_object.text=="") {
    ViewportCoords start =
      viewportCoordsFromDiagramCoords(text_object.position);
    ViewportCoords end = start;
    return ViewportRect{start,end};
  }

  return nodeRect(text_object);
}


ViewportCoords
  DiagramEditor::alignmentPoint(
    const ViewportRect &rect,
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
  return ViewportCoords(x,y);
}


ViewportTextObject
  DiagramEditor::alignedTextObject(
    const std::string &text,
    const ViewportCoords &position,
    float horizontal_alignment,
    float vertical_alignment
  ) const
{
  ViewportTextObject text_object;
  text_object.text = text;
  text_object.position = ViewportCoords(0,0);
  ViewportRect rect = rectAroundText(text_object);
  Vector2D offset =
    alignmentPoint(rect,horizontal_alignment,vertical_alignment) -
    ViewportCoords(0,0);
  text_object.position = position - offset;
  return text_object;
}


ViewportRect
  DiagramEditor::nodeBodyRect(
    const Node &node,
    const ViewportRect &header_rect
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
    ViewportRect r =
      rectAroundText(
        alignedTextObject(
          s,
          ViewportCoords(left_x,bottom_y),
          /*horizontal_alignment*/0,
          /*vertical_alignment*/1
        )
      );
    bottom_y = r.start.y;
  }

  int n_inputs = node.nInputs();
  int n_lines = strings.size();

  while (n_lines<n_inputs) {
    ViewportRect r =
      rectAroundText(
        alignedTextObject(
          "$",
          ViewportCoords(left_x,bottom_y),
          /*horizontal_alignment*/0,
          /*vertical_alignment*/1
        )
      );
    bottom_y = r.start.y;
    ++n_lines;
  }

  // The right side is the maximum of all the right sides of the individual
  // text objects.
  float right_x = header_rect.end.x;

  for (const auto &s : strings) {
    ViewportRect r =
      rectAroundText(
        alignedTextObject(
          s,
          ViewportCoords(left_x,top_y),
          /*horizontal_alignment*/0,
          /*vertical_alignment*/1
        )
      );
    if (r.end.x > right_x) {
      right_x = r.end.x;
    }
  }

  ViewportRect body_rect;

  body_rect.start = ViewportCoords(left_x,bottom_y);
  body_rect.end = ViewportCoords(right_x,top_y);

  return body_rect;
}


ViewportTextObject
  DiagramEditor::inputTextObject(
    const string &s,
    float left_x,
    float y
  ) const
{
  return
    alignedTextObject(
      s,
      ViewportCoords(left_x,y),
      /*horizontal_alignment*/0,
      /*vertical_alignment*/1
    );
}


NodeRenderInfo DiagramEditor::nodeRenderInfo(const Node &node) const
{
  const DiagramTextObject &header_text_object = node.header_text_object;

  // We need to determine a rectangle that fits around the contents.
  ViewportRect header_rect = nodeHeaderRect(header_text_object);

  NodeRenderInfo render_info;
  ViewportRect body_rect = nodeBodyRect(node,header_rect);
  render_info.header_rect = header_rect;

  float left_x = body_rect.start.x;
  float right_x = body_rect.end.x;

  float margin = 5;

  float left_outer_x = left_x - margin;
  float right_outer_x = right_x + margin;

  float top_y = body_rect.end.y;

  size_t n_lines = node.lines.size();
  float input_bottom_y = 0;

  {
    float y = top_y;
    int n_inputs = node.nInputs();

    for (int i=0; i!=n_inputs; ++i) {
      ViewportTextObject t = inputTextObject("$",left_x,y);
      ViewportRect r = rectAroundText(t);
      float line_start_y = r.start.y;
      float line_end_y = r.end.y;
      float line_center_y = (line_start_y + line_end_y)/2;
      float connector_x = (left_outer_x - connector_radius - 5);
      float connector_y = line_center_y;

      Circle c;
      c.center = Point2D(connector_x,connector_y);
      c.radius = connector_radius;
      render_info.input_connector_circles.push_back(c);

      y = r.start.y;
    }

    input_bottom_y = y;
  }

  vector<float> line_start_ys(n_lines);
  vector<float> line_end_ys(n_lines);
  float line_bottom_y = 0;

  {
    float y = top_y;

    for (size_t line_index=0; line_index!=n_lines; ++line_index) {
      const auto &line = node.lines[line_index];
      ViewportTextObject t = inputTextObject(line.text,left_x,y);
      render_info.text_objects.push_back(t);
      ViewportRect r = rectAroundText(t);
      line_start_ys[line_index] = r.start.y;
      line_end_ys[line_index] = r.end.y;
      y = r.start.y;
    }

    line_bottom_y = y;
  }

  float text_offset = 0;

  if (input_bottom_y < line_bottom_y) {
    text_offset = (line_bottom_y - input_bottom_y)/2;
  }

  for (size_t line_index=0; line_index!=n_lines; ++line_index) {
    render_info.text_objects[line_index].position.y -= text_offset;
    line_start_ys[line_index] -= text_offset;
    line_end_ys[line_index] -= text_offset;
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


bool
  DiagramEditor::nodeContains(
    NodeIndex node_index,
    const ViewportCoords &p
  ) const
{
  Node &node = diagram().node(node_index);
  NodeRenderInfo render_info = nodeRenderInfo(node);

  if (render_info.header_rect.contains(p)) {
    return true;
  }

  if (render_info.body_outer_rect.contains(p)) {
    return true;
  }

  return false;
}


NodeIndex DiagramEditor::indexOfNodeContaining(const ViewportCoords &p) const
{
  for (NodeIndex i : diagram().existingNodeIndices()) {
    if (nodeContains(i,p)) {
      return i;
    }
  }

  return noNodeIndex();
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


bool
  DiagramEditor::nodeIsInRect(
    NodeIndex node_index,
    const ViewportRect &rect
  ) const
{
  Node &node = diagram().node(node_index);
  NodeRenderInfo render_info = nodeRenderInfo(node);

  return rect.contains(render_info.body_outer_rect);
}


void DiagramEditor::selectNodesInRect(const ViewportRect &rect)
{
  for (NodeIndex index : diagram().existingNodeIndices()) {
    if (nodeIsInRect(index,rect)) {
      alsoSelectNode(index);
    }
  }
}


static void order(float &a,float &b)
{
  if (a>b) {
    std::swap(a,b);
  }
}


int
  DiagramEditor::closestColumn(
    const ViewportTextObject &line_text_object,
    const ViewportCoords &p
  ) const
{
  int best_column_index = 0;
  int length = line_text_object.text.length();
  float min_distance = FLT_MAX;

  for (int column_index=0; column_index!=length+1; ++column_index) {
    ViewportLine l = textObjectCursorLine(line_text_object,column_index);
    float d = fabsf(l.start.x - p.x);

    if (d < min_distance) {
      min_distance = d;
      best_column_index = column_index;
    }
  }

  return best_column_index;
}


NodeTextEditor::CursorPosition
  DiagramEditor::closestCursorPositionTo(
    NodeIndex node_index,
    const ViewportCoords &p
  ) const
{
  NodeRenderInfo render_info = nodeRenderInfo(node(node_index));
  int line_index = 0;
  int n_lines = render_info.text_objects.size();

  assert(n_lines!=0);

  for (;line_index != n_lines; ++line_index) {
    const ViewportTextObject &line_text_object =
      render_info.text_objects[line_index];

    ViewportRect line_rect = rectAroundText(line_text_object);

    if (p.y >= line_rect.start.y && p.y <= line_rect.end.y) {
      int best_column_index = closestColumn(line_text_object,p);

      return {line_index,best_column_index};
    }
  }

  {
    int line_index = 0;

    const ViewportTextObject &line_text_object =
      render_info.text_objects[line_index];

    ViewportRect line_rect = rectAroundText(line_text_object);

    if (p.y > line_rect.end.y) {
      return {line_index,closestColumn(line_text_object,p)};
    }
  }

  assert(false);
}


void DiagramEditor::mouseReleasedAt(ViewportCoords mouse_release_position)
{
  if (mouse_mode==MouseMode::translate_view) {
    mouse_mode = MouseMode::none;
    return;
  }

  if (!selected_node_connector_index.isNull()) {
    NodeConnectorIndex release_index =
      indexOfNodeConnectorContaining(mouse_release_position);

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
    else {
      if (selected_node_connector_index.input_index>=0) {
        int input_node_index = selected_node_connector_index.node_index;
        int input_index = selected_node_connector_index.input_index;
        connectNodes(nullNodeIndex(),0,input_node_index,input_index);
      }
    }
    selected_node_connector_index.clear();
    notifyDiagramChanged();
    redraw();
    return;
  }

  if (maybe_selection_rectangle) {
    maybe_selection_rectangle->end = mouse_release_position;
    ViewportRect selection_rect = *maybe_selection_rectangle;

    order(selection_rect.start.x,selection_rect.end.x);
    order(selection_rect.start.y,selection_rect.end.y);

    maybe_selection_rectangle.reset();

    if (selection_rect.start!=selection_rect.end) {
      selectNodesInRect(selection_rect);
      redraw();
      return;
    }
  }

  if (mouse_press_position==mouse_release_position) {
    if (!aNodeIsFocused() && !aNodeIsSelected()) {
      int new_node_index =
        addNode("",diagramCoordsFromViewportCoords(mouse_press_position));
      focusNode(new_node_index,diagram());
      notifyDiagramChanged();
      redraw();
      return;
    }

    if (node_was_selected && selectedNodeIndex()!=noNodeIndex()) {
      focusNode(selectedNodeIndex(),diagram());
      clearSelection();
      redraw();
      return;
    }
  }
}


void DiagramEditor::clearFocus()
{
  if (aNodeIsFocused()) {
    NodeIndex old_focused_node_index = focused_node_index;
    Node &focused_node = focusedNode(diagram());
    unfocus();
    if (focused_node.isEmpty()) {
      deleteNode(old_focused_node_index);
    }
  }
}


void DiagramEditor::clearSelection()
{
  selected_node_indices.clear();
}


void
  DiagramEditor::middleMousePressedAt(
    ViewportCoords p,
    EventModifiers modifiers
  )
{
  if (modifiers.alt_is_pressed) {
    mouse_mode = MouseMode::translate_view;
    mouse_press_position = p;
    mouse_down_view_offset = view_offset;
  }
}


void
  DiagramEditor::leftMousePressedAt(
    ViewportCoords p,
    EventModifiers modifiers
  )
{
  if (!diagramPtr()) {
    return;
  }

  mouse_press_position = p;

  node_was_selected = false;

  if (aNodeIsFocused()) {
    if (nodeContains(focused_node_index,p)) {
      NodeTextEditor::CursorPosition new_position =
        closestCursorPositionTo(focused_node_index,p);
      text_editor.moveCursorTo(new_position);
      redraw();
      return;
    }
  }

  clearFocus();

  {
    int i = indexOfNodeContaining(p);

    if (i>=0) {
      node_was_selected = nodeIsSelected(i);

      if (modifiers.shift_is_pressed) {
        alsoSelectNode(i);
      }
      else {
        if (!node_was_selected) {
          setSelectedNodeIndex(i);
        }
        else {
          // Focusing the selected node happens on a mouse release instead
          // of on a mouse press since we have to handle the case where the
          // user drags the selected node.
        }
      }

      if (modifiers.ctrl_is_pressed) {
        vector<NodeIndex> new_node_indices =
          diagram().duplicateNodes(selected_node_indices);
        selected_node_indices = new_node_indices;
      }

      focused_node_index = -1;
      original_node_positions.clear();

      for (auto node_index : selected_node_indices) {
        original_node_positions[node_index] =
          node(node_index).header_text_object.position;
      }

      redraw();
      return;
    }
  }

  clearSelection();
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

  maybe_selection_rectangle = ViewportRect();
  maybe_selection_rectangle->start = maybe_selection_rectangle->end = p;

  redraw();
}


int DiagramEditor::nSelectedNodes() const
{
  return selected_node_indices.size();
}


void DiagramEditor::selectNode(NodeIndex node_index)
{
  if (aNodeIsFocused()) {
    unfocus();
  }

  setSelectedNodeIndex(node_index);
}


bool DiagramEditor::aNodeIsFocused() const
{
  return focused_node_index!=noNodeIndex();
}


void DiagramEditor::focusNode(int node_index,Diagram &diagram)
{
  focused_node_index = node_index;
  text_editor.beginEditing(focusedNode(diagram));
}


Node& DiagramEditor::focusedNode(Diagram &diagram)
{
  return diagram.node(focused_node_index);
}


void DiagramEditor::alsoSelectNode(NodeIndex arg)
{
  selected_node_indices.push_back(arg);
}


template <typename T>
static bool contains(const vector<T> &v,const T& a)
{
  return std::find(v.begin(),v.end(),a)!=v.end();
}


bool DiagramEditor::nodeIsSelected(NodeIndex arg)
{
  return contains(selected_node_indices,arg);
}


bool DiagramEditor::aNodeIsSelected() const
{
  return !selected_node_indices.empty();
}


void DiagramEditor::mouseMovedTo(const ViewportCoords &mouse_position)
{
  if (mouse_mode==MouseMode::translate_view) {
    view_offset =
      mouse_down_view_offset + (mouse_position - mouse_press_position);
    redraw();
    return;
  }

  if (!selected_node_connector_index.isNull()) {
    temp_source_pos = mouse_position;
    redraw();
    return;
  }

  if (aNodeIsSelected()) {
    for (NodeIndex i : selected_node_indices) {
      assert(original_node_positions.count(i));
      node(i).header_text_object.position =
        original_node_positions[i] + (mouse_position - mouse_press_position);
    }
    redraw();
    return;
  }

  if (maybe_selection_rectangle) {
    maybe_selection_rectangle->end = mouse_position;
    redraw();
  }
}


void DiagramEditor::exportDiagramPressed()
{
  string path = askForSavePath();

  if (path=="") {
    return;
  }

  ofstream stream(path);

  if (!stream) {
    string message = "Unable to create "+path;
    showError(message);
    return;
  }

  printDiagramOn(stream,diagram());
}


void DiagramEditor::importDiagramPressed()
{
  string path = askForOpenPath();
  ifstream stream(path);

  if (!stream) {
    showError("Unable to open "+path);
    return;
  }

  string error;
  Diagram new_diagram;
  scanDiagramFrom(stream,new_diagram,error);

  if (!error.empty()) {
    showError(error);
    return;
  }

  diagram() = new_diagram;

  notifyDiagramChanged();
}


void DiagramEditor::setSelectedNodeIndex(NodeIndex arg)
{
  if (arg==noNodeIndex()) {
    selected_node_indices.clear();
  }
  else {
    selected_node_indices.resize(1);
    selected_node_indices[0] = arg;
  }
}


NodeIndex DiagramEditor::selectedNodeIndex() const
{
  if (selected_node_indices.size()!=1) {
    return noNodeIndex();
  }

  return selected_node_indices[0];
}


auto
  DiagramEditor::viewportCoordsFromDiagramCoords(
    const Point2D &diagram_coords
  ) const -> ViewportCoords
{
  return ViewportCoords(diagram_coords + view_offset);
}


DiagramCoords
  DiagramEditor::diagramCoordsFromViewportCoords(
    const ViewportCoords &viewport_coords
  ) const
{
  return DiagramCoords(viewport_coords - view_offset);
}


ViewportLine
  DiagramEditor::cursorLine(
    const Node &node,
    const NodeTextEditor::CursorPosition cursor_position
  )
{
  NodeRenderInfo render_info = nodeRenderInfo(node);
  int line_index = cursor_position.line_index;
  int column_index = cursor_position.column_index;
  ViewportLine cursor_line =
    textObjectCursorLine(render_info.text_objects[line_index],column_index);
  return cursor_line;
}


ViewportLine
  DiagramEditor::cursorLine(
    NodeIndex focused_node_index,
    NodeTextEditor::CursorPosition cursor_position
  )
{
  return cursorLine(node(focused_node_index),cursor_position);
}


string DiagramEditor::lineError(NodeIndex node_index,int line_index) const
{
  return diagram_state.node_states[node_index].line_errors[line_index];
}


auto DiagramEditor::maybeNodeLineAt(const ViewportCoords &p) const
  -> Optional<NodeLineIndex>
{
  int i = indexOfNodeContaining(p);

  if (i<0) {
    return {};
  }

  NodeTextEditor::CursorPosition new_position =
    closestCursorPositionTo(i,p);

  return NodeLineIndex{i,new_position.line_index};
}


Optional<string>
  DiagramEditor::maybeToolTipTextAt(const ViewportCoords &p) const
{
  Optional<NodeLineIndex> maybe_node_line_index = maybeNodeLineAt(p);

  if (!maybe_node_line_index) {
    return {};
  }

  NodeIndex node_index = maybe_node_line_index->node_index;
  int line_index = maybe_node_line_index->line_index;

  string error_message = lineError(node_index,line_index);

  if (!error_message.empty()) {
    assert(error_message.back()=='\n');
    error_message.pop_back();
  }

  return error_message;
}
