#include "diagrameditor.hpp"

#include <cassert>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iostream>
#include "ostreamvector.hpp"
#include "diagramio.hpp"
#include "size2d.hpp"

#define ADD_CALCULATE_NODE_LAYOUT 0

using std::string;
using std::vector;
using std::cerr;
using std::ifstream;
using std::ofstream;
using Node = DiagramNode;


static ViewportRect withMargin(const ViewportRect &rect,float margin)
{
  auto offset = ViewportVector{margin,margin};
  return {rect.start-offset, rect.end+offset};
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


void DiagramEditor::setDiagramStatePtr(const DiagramState *arg)
{
  diagram_state_ptr = arg;

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
        setDiagramStatePtr(&diagram_observer_ptr->diagramState());
      };
  }

  if (diagram_observer_ptr) {
    setDiagramPtr(&diagram_observer_ptr->diagram());
    setDiagramStatePtr(&diagram_observer_ptr->diagramState());
  }
  else {
    setDiagramPtr(nullptr);
    setDiagramStatePtr(nullptr);
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


ViewportSize DiagramEditor::textSize(const string &text) const
{
  return rectAroundText(text).size();
}


ViewportRect
  DiagramEditor::rectAroundTextObject(
    const ViewportTextObject &text_object
  ) const
{
  ViewportVector offset = text_object.position - ViewportCoords{0,0};
  return rectAroundText(text_object.text) + offset;
}


ViewportRect
  DiagramEditor::nodeRect(const DiagramTextObject &text_object) const
{
  return withMargin(rectAroundTextObject(viewportTextObject(text_object)),5);
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
  ViewportRect rect = rectAroundTextObject(text_object);
  ViewportVector offset =
    alignmentPoint(rect,horizontal_alignment,vertical_alignment) -
    ViewportCoords(0,0);
  text_object.position = position - offset;
  return text_object;
}


ViewportTextObject
  DiagramEditor::inputTextObject(
    const string &s,
    float left_x,
    float top_y
  ) const
{
  return
    alignedTextObject(
      s,
      ViewportCoords(left_x,top_y),
      /*horizontal_alignment*/0,
      /*vertical_alignment*/1
    );
}


// Instead of calling inputTextObject, we could pass the input text
// objects for each line, then we could make this static.
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

  vector<string> line_texts = node.lineTexts();

  float bottom_y = top_y;

  vector<ViewportRect> line_rects;

  // Make a rectangle for each string
  for (const auto& s : line_texts) {
    ViewportRect r = rectAroundTextObject( inputTextObject(s,left_x,bottom_y) );
    line_rects.push_back(r);
    bottom_y = r.start.y;
  }

  int n_inputs = node.nInputs();
  int n_lines = line_texts.size();

  // Extend the bottom if we have more inputs than lines.
  while (n_lines < n_inputs) {
    ViewportRect r =
      rectAroundTextObject( inputTextObject("$",left_x,bottom_y) );
    bottom_y = r.start.y;
    ++n_lines;
  }

  // The right side is the maximum of all the right sides of the individual
  // text objects.
  float right_x = header_rect.end.x;

  for (const auto &r : line_rects) {
    if (r.end.x > right_x) {
      right_x = r.end.x;
    }
  }

  ViewportRect body_rect;

  body_rect.start = ViewportCoords(left_x,bottom_y);
  body_rect.end = ViewportCoords(right_x,top_y);

  return body_rect;
}


namespace {
struct NodeLayoutParams {
  using Length = float;
  using Coord = float;
  vector<int> line_n_inputs;
  vector<ViewportSize> line_text_sizes;
  ViewportSize connector_size;
  Coord left_x;
  Coord top_y;
};
}


#if ADD_CALCULATE_NODE_LAYOUT
namespace {
struct NodeLayout {
  vector<ViewportRect> lines;
  vector<ViewportRect> inputs;
  vector<ViewportRect> outputs;
  ViewportSize body_size;
};
}
#endif


namespace {
template <typename Index>
struct Range {
  Index begin_index;
  Index end_index;

  struct iterator {
    Index index;

    bool operator==(iterator arg) const { return index==arg.index; }
    bool operator!=(iterator arg) const { return !operator==(arg); }
    iterator& operator++() { ++index; return *this; }
    Index operator*() const { return index; }
  };

  iterator begin() const { return iterator{begin_index}; }
  iterator end() const { return iterator{end_index}; }
};
}


template <typename Begin,typename End>
static auto range(Begin begin,End end)
{
  using CommonType = std::common_type_t<Begin,End>;
  const CommonType common_begin = begin;
  const CommonType common_end = end;
  return Range<CommonType>{common_begin,common_end};
}


template <typename Container>
static auto sumOf(const Container &container)
{
  using Value = decltype(container[0]);
  return std::accumulate( container.begin(), container.end(), Value{});
}


template <typename Container>
static auto maxOf(const Container &container)
{
  auto iter = std::max_element(container.begin(), container.end());
  assert(iter != container.end());
  return *iter;
}


using Rect = TaggedRect<void>;


#if ADD_CALCULATE_NODE_LAYOUT
static ViewportRect makeRect(ViewportCoords position,ViewportSize size)
{
  return {position, position + size};
}
#endif


#if ADD_CALCULATE_NODE_LAYOUT
static vector<float>
  transformed(
    const vector<ViewportSize> &sizes,
    const float ViewportSize::*member_ptr
  )
{
  vector<float> result;

  std::transform(
    sizes.begin(),
    sizes.end(),
    inserter(result,result.begin()),
    std::mem_fn(member_ptr)
  );

  return result;
}
#endif


#if ADD_CALCULATE_NODE_LAYOUT
static NodeLayout calculateNodeLayout(const NodeLayoutParams &arg)
{
  // Create regions where each region is the area that the line text
  // will be drawn inside.  These regions will have a height that is
  // the maximum of the line text height and the height necessary for
  // all the inputs.
  using Length = float;
  using Coord = float;
  using Index = int;
  const auto n_lines = arg.line_n_inputs.size();
  const auto n_inputs = sumOf(arg.line_n_inputs);
  const auto &line_text_sizes = arg.line_text_sizes;
  const auto line_text_widths =
    transformed(arg.line_text_sizes,&ViewportSize::x);
  const auto left_x = arg.left_x;
  const auto top_y = arg.top_y;
  Length connector_width = arg.connector_size.x;
  Length connector_height = arg.connector_size.y;
  const vector<int> &line_n_inputs = arg.line_n_inputs;
  vector<Length> region_heights(n_lines);
  vector<Coord> input_ys(n_inputs);
  vector<Coord> region_ys(n_lines);
  vector<Coord> output_ys(n_lines);
  vector<Coord> line_text_ys(n_lines);
  vector<Index> first_input_indices(n_lines);
  const auto input_x = left_x - connector_width;
  const auto input_width = connector_width;
  const auto input_height = connector_height;
  const auto output_width = connector_width;
  const auto output_height = connector_height;
  const auto region_x = left_x;

  for (auto i : range(0,n_lines)) {
    // Determine the required height for the inputs of line i.
    Length line_input_height = connector_height * line_n_inputs[i];

    region_heights[i] = std::max(line_input_height,line_text_sizes[i].y);
    region_ys[i] = (i==0) ? top_y : region_ys[i-1] - region_heights[i-1];
    first_input_indices[i] =
      (i==0) ? 0 : first_input_indices[i-1] + line_n_inputs[i-1];

    // Lay out the inputs vertically
    for (auto j : range(0,line_n_inputs[i])) {
      if (j==0) {
        input_ys[first_input_indices[i] + j] =
          region_ys[i] + (region_heights[i] - line_input_height)/2;
      }
      else {
        input_ys[first_input_indices[i] + j] =
          input_ys[first_input_indices[i]+j-1] + input_height;
      }
    }

    // Lay out the line texts vertically
    line_text_ys[i] =
      region_ys[i] - (region_heights[i] - line_text_sizes[i].y)/2;
  }

  Length body_height = sumOf(region_heights);
  Length body_width = maxOf(line_text_widths);
  const auto output_x = region_x + body_width;

  for (auto i : range(0,n_lines)) {
    output_ys[i] = region_ys[i] - (region_heights[i] - connector_height)/2;
  }

  vector<ViewportRect> line_rects;
  vector<ViewportRect> input_rects;
  vector<ViewportRect> output_rects;

  for (auto i : range(0,n_lines)) {
    line_rects.push_back(
      makeRect(
        ViewportCoords(region_x,region_ys[i]),
        ViewportSize(line_text_widths[i],line_text_sizes[i].y)
      )
    );

    input_rects.push_back(
      makeRect(
        ViewportCoords(input_x,input_ys[i]),
        ViewportSize(input_width,input_height)
      )
    );

    output_rects.push_back(
      makeRect(
        ViewportCoords(output_x,output_ys[i]),
        ViewportSize(output_width,output_height)
      )
    );
  }

  ViewportSize body_size = {body_width,body_height};
  return NodeLayout{line_rects,input_rects,output_rects,body_size};
}
#endif


#if !ADD_CALCULATE_NODE_LAYOUT
NodeRenderInfo DiagramEditor::nodeRenderInfo(const Node &node) const
{
  const float connector_distance = 5;
    // Distance between the connector circles and the node body.
  const DiagramTextObject &header_text_object = node.header_text_object;

  // We need to determine a rectangle that fits around the contents.
  ViewportRect header_rect = nodeHeaderRect(header_text_object);

  ViewportRect body_rect = nodeBodyRect(node, header_rect);
  // We should be able to determine the left_x and top_y from the header_rect
  // directly.
  float left_x = body_rect.start.x;
  float right_x = body_rect.end.x;
  float top_y = body_rect.end.y;

  float margin = 5;
    // Horizontal distance between the text of the node lines and the
    // outer rectangle of the node body.

  float left_outer_x = left_x - margin;
  float right_outer_x = right_x + margin;


  size_t n_lines = node.lines.size();
  float input_bottom_y = 0;

  vector<Circle> input_connector_circles;

  { // Add input connector circles
    float y = top_y;
    int n_inputs = node.nInputs();

    for (int i=0; i!=n_inputs; ++i) {
      ViewportTextObject t = inputTextObject("$", left_x, y);
      ViewportRect r = rectAroundTextObject(t);
      float line_start_y = r.start.y;
      float line_end_y = r.end.y;
      float line_center_y = (line_start_y + line_end_y)/2;
      float connector_x =
        (left_outer_x - connector_radius - connector_distance);
      float connector_y = line_center_y;

      Circle c;
      c.center = ViewportCoords(connector_x,connector_y);
      c.radius = connector_radius;
      input_connector_circles.push_back(c);

      y = line_start_y;
    }

    input_bottom_y = y;
  }

  vector<float> line_start_ys(n_lines);
  vector<float> line_end_ys(n_lines);
  float line_bottom_y = 0;
  vector<ViewportTextObject> text_objects;

  { // Add text objects for each line.
    float y = top_y;

    for (size_t line_index=0; line_index!=n_lines; ++line_index) {
      const auto &line_text = node.lines[line_index].text;
      ViewportTextObject t = inputTextObject(line_text, left_x, y);
      ViewportRect r = rectAroundTextObject(t);
      text_objects.push_back(t);
      line_start_ys[line_index] = r.start.y;
      line_end_ys[line_index] = r.end.y;
      y = r.start.y;
    }

    line_bottom_y = y;
  }

  // If the height of the inputs is greater than the height of the lines,
  // then we'll offset the lines vertically so that they are centered.
  float text_offset = 0;

  if (input_bottom_y < line_bottom_y) {
    text_offset = (line_bottom_y - input_bottom_y)/2;
  }

  for (size_t line_index=0; line_index!=n_lines; ++line_index) {
    text_objects[line_index].position.y -= text_offset;
    line_start_ys[line_index] -= text_offset;
    line_end_ys[line_index] -= text_offset;
  }

  vector<Circle> output_connector_circles;

  // Add output connector circles
  {
    size_t statement_index = 0;
    size_t n_statements = node.statements.size();
    size_t line_index = 0;

    for (; statement_index != n_statements; ++statement_index) {
      const auto &statement = node.statements[statement_index];
      const auto statement_n_lines = statement.n_lines;
      float expression_start_y =
        line_start_ys[line_index + statement_n_lines - 1];
      float expression_end_y = line_end_ys[line_index];
      float expression_center_y = (expression_start_y + expression_end_y)/2;

      if (statement.has_output) {
        // What is this 5?
        float connector_x =
          (right_outer_x + connector_radius + connector_distance);
        float connector_y = expression_center_y;

        Circle c;
        c.center = ViewportCoords(connector_x,connector_y);
        c.radius = connector_radius;
        output_connector_circles.push_back(c);
      }

      line_index += statement_n_lines;
    }
  }

  NodeRenderInfo render_info;

  render_info.header_rect = header_rect;
  render_info.body_outer_rect = body_rect;
  render_info.body_outer_rect.start.x = left_outer_x;
  render_info.body_outer_rect.end.x = right_outer_x;
  render_info.input_connector_circles = input_connector_circles;
  render_info.output_connector_circles = output_connector_circles;
  render_info.text_objects = text_objects;

  return render_info;
}
#else
NodeRenderInfo DiagramEditor::nodeRenderInfo2(const Node &node) const
{
  const DiagramTextObject &header_text_object = node.header_text_object;
  ViewportRect header_rect = nodeHeaderRect(header_text_object);

  NodeLayoutParams params;

  for (auto &line : node.lines) {
    params.line_n_inputs.push_back(line.n_inputs);
    params.line_text_sizes.push_back(textSize(line.text));
  }

  params.connector_size = {connector_radius*2, connector_radius*2};
  params.left_x =

  calculateNodeLayout();

  NodeRenderInfo render_info;

  render_info.header_rect = header_rect;
  render_info.body_outer_rect = body_rect;
  render_info.body_outer_rect.start.x = left_outer_x;
  render_info.body_outer_rect.end.x = right_outer_x;
  render_info.input_connector_circles = input_connector_circles;
  render_info.output_connector_circles = output_connector_circles;
  render_info.text_objects = text_objects;

  return render_info;
}
#endif


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
    const ViewportCoords &p
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
    const ViewportCoords &p
  )
{
  return nodeOutputCircle(node(node_index),output_index).contains(p);
}


NodeConnectorIndex
  DiagramEditor::indexOfNodeConnectorContaining(const ViewportCoords &p)
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


struct DiagramEditor::CursorPositionFinder {
  const DiagramEditor &diagram_editor;
  const NodeRenderInfo &render_info;
  const ViewportCoords &p;

  template <typename IsFeasiblePointFunction>
  Optional<NodeTextEditor::CursorPosition>
    maybeFind(
      int line_index,
      const IsFeasiblePointFunction &is_feasible_point
    )
  {
    const ViewportTextObject &line_text_object =
      render_info.text_objects[line_index];

    ViewportRect line_rect =
      diagram_editor.rectAroundTextObject(line_text_object);

    if (is_feasible_point(line_rect)) {
      int best_column_index = diagram_editor.closestColumn(line_text_object,p);

      return {{line_index,best_column_index}};
    }

    return {};
  }

  Optional<NodeTextEditor::CursorPosition> maybeFindInside(int line_index)
  {
    auto inside = [&](const ViewportRect &line_rect)
    {
      return (p.y >= line_rect.start.y && p.y <= line_rect.end.y);
    };

    return maybeFind(line_index,inside);
  }

  Optional<NodeTextEditor::CursorPosition> maybeFindAbove(int line_index)
  {
    auto above = [&](const ViewportRect &line_rect)
    {
      return (p.y > line_rect.end.y);
    };

    return maybeFind(line_index,above);
  }

  Optional<NodeTextEditor::CursorPosition> maybeFindBelow(int line_index)
  {
    auto below = [&](const ViewportRect &line_rect){
      return (p.y < line_rect.start.y);
    };

    return maybeFind(line_index,below);
  }
};


NodeTextEditor::CursorPosition
  DiagramEditor::closestCursorPositionTo(
    NodeIndex node_index,
    const ViewportCoords &p
  ) const
{
  NodeRenderInfo render_info = nodeRenderInfo(node(node_index));
  int n_lines = render_info.text_objects.size();

  assert(n_lines!=0);

  CursorPositionFinder finder{*this,render_info,p};

  for (int line_index=0; line_index != n_lines; ++line_index) {
    if (auto maybe_cursor_position = finder.maybeFindInside(line_index)) {
      return *maybe_cursor_position;
    }
  }

  int first_line = 0;

  if (auto maybe_cursor_position = finder.maybeFindAbove(first_line)) {
    return *maybe_cursor_position;
  }

  int last_line = n_lines - 1;

  if (auto maybe_cursor_position = finder.maybeFindBelow(last_line)) {
    return *maybe_cursor_position;
  }

  // Uh... where?
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
        original_node_positions[i] +
        diagramVectorFromViewportVector(mouse_position - mouse_press_position);
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


DiagramVector
  DiagramEditor::diagramVectorFromViewportVector(
    const ViewportVector &arg
  ) const
{
  // We can't scale the view yet, so a diagram vector and viewport vector
  // are always the same.
  return DiagramVector{arg.x,arg.y};
}


auto
  DiagramEditor::viewportCoordsFromDiagramCoords(
    // This should be DiagramCoords
    const DiagramCoords &diagram_coords
  ) const -> ViewportCoords
{
  return ViewportCoords(Point2D(diagram_coords) + Vector2D(view_offset));
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
  if (diagram_state_ptr) {
    return diagram_state_ptr->node_states[node_index].line_errors[line_index];
  }
  else {
    // We requested the line error for a diagram when we have no diagram
    // state.  It seems like this shouldn't happen in practice.  If the
    // diagram editor window is open, it should be observing a diagram, and
    // that diagram should be evaluated, in which case it has a state.
    assert(false);
  }
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
