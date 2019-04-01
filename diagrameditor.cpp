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


static constexpr float connector_radius = 5;


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
    const DiagramPoint &position
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


static ViewportRect
  offsetTextRect(const ViewportRect &text_rect,const ViewportPoint &position)
{
  ViewportVector offset = position - ViewportPoint{0,0};
  return text_rect + offset;
}


ViewportRect
  DiagramEditor::rectAroundTextObject(
    const ViewportTextObject &text_object
  ) const
{
  return offsetTextRect(rectAroundText(text_object.text),text_object.position);
}


ViewportRect
  DiagramEditor::nodeHeaderRect(
    const DiagramTextObject &diagram_text_object
  ) const
{
  string header_text = diagram_text_object.text;

  ViewportPoint result_position =
    viewportCoordsFromDiagramCoords(diagram_text_object.position);

  if (header_text == "") {
    return ViewportRect{result_position,result_position};
  }

  ViewportRect text_rect = rectAroundText(header_text);
  ViewportRect inner_rect = offsetTextRect(text_rect,result_position);

  return withMargin(inner_rect,5);
}


static ViewportPoint
  alignmentPoint(
    const ViewportRect &rect,
    float horizontal_alignment,
    float vertical_alignment
  )
{
  float h = horizontal_alignment;
  float v = vertical_alignment;
  float x1 = rect.start.x;
  float x2 = rect.end.x;
  float y1 = rect.start.y;
  float y2 = rect.end.y;
  float x = x1*(1-h) + x2*h;
  float y = y1*(1-v) + y2*v;
  return ViewportPoint(x,y);
}


static ViewportVector
  alignmentOffset(
    const ViewportRect &rect,
    float horizontal_alignment,
    float vertical_alignment
  )
{
  return
    ViewportPoint(0,0) -
    alignmentPoint(rect,horizontal_alignment,vertical_alignment);
}


static ViewportPoint
  alignedTextPosition(
    const ViewportRect &rect,
    const ViewportPoint &position,
    float horizontal_alignment,
    float vertical_alignment
  )
{
  ViewportVector offset =
    alignmentOffset(rect, horizontal_alignment, vertical_alignment);
  return position + offset;
}


// This gives the position if I place the upper left
// corner of the rectangle at a certain position.
static ViewportPoint
  inputTextPosition(
    const ViewportRect &rect,
    float left_x,
    float top_y
  )
{
  ViewportPoint position(left_x,top_y);
  float horizontal_alignment = 0;
  float vertical_alignment = 1;
  return
    alignedTextPosition(rect,position,horizontal_alignment,vertical_alignment);
}


ViewportTextObject
  DiagramEditor::inputTextObject(
    const string &text,
    float left_x,
    float top_y
  ) const
{
  ViewportRect rect = rectAroundText(text);
  ViewportPoint aligned_position = inputTextPosition(rect,left_x,top_y);
  return ViewportTextObject{text, aligned_position};
}


static ViewportPoint nodeBodyTopLeft(const ViewportRect &header_rect)
{
  // The top of the rectangle should be the bottom of the header text
  // object.
  float top_y = header_rect.start.y;

  // The left side should be the left of the header rect
  float left_x = header_rect.start.x;

  return ViewportPoint{left_x,top_y};
}


static vector<ViewportPoint>
  calculateLineTextPositions(
    const ViewportPoint &top_left,
    const vector<ViewportRect> &line_text_rects
  )
{
  float top_y = top_left.y;
  float left_x = top_left.x;
  vector<ViewportPoint> line_text_positions;
  float y = top_y;

  for (const auto& rect : line_text_rects) {
    ViewportPoint aligned_position = inputTextPosition(rect,left_x,y);
    line_text_positions.push_back(aligned_position);
    y = offsetTextRect(rect,aligned_position).start.y;
  }

  return line_text_positions;
}


static vector<ViewportRect>
  calculateLineRects(
    const vector<ViewportRect> &line_text_rects,
    const vector<ViewportPoint> &line_text_positions
  )
{
  vector<ViewportRect> line_rects;

  int n_lines = line_text_rects.size();

  // Make a rectangle for each string
  for (int line_index = 0; line_index != n_lines; ++line_index) {
    const auto &aligned_position = line_text_positions[line_index];
    const auto rect = line_text_rects[line_index];
    line_rects.push_back(offsetTextRect(rect,aligned_position));
  }

  return line_rects;
}


static float
  calculateRightX(
    const ViewportRect &header_rect,
    const vector<ViewportRect> &line_rects
  )
{
  // The right side is the maximum of all the right sides of the individual
  // line rectangles.
  float right_x = header_rect.end.x;

  for (const auto &r : line_rects) {
    if (r.end.x > right_x) {
      right_x = r.end.x;
    }
  }

  return right_x;
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


static vector<ViewportRect>
  calculateInputConnectorRects(
    const float left_outer_x,
    const float top_y,
    const float connector_distance,
    const float connector_radius,
    int n_inputs,
    const float line_height
  )
{
  vector<ViewportRect> input_connector_rects;
  float y = top_y;

  for (int i=0; i!=n_inputs; ++i) {
    float connector_bottom_y = y - line_height;
    float connector_top_y = y;
    float connector_right_x = left_outer_x - connector_distance;
    float connector_left_x = connector_right_x - connector_radius*2;
    ViewportRect
      connector_rect{
        {connector_left_x,connector_bottom_y},
        {connector_right_x,connector_top_y}
      };
    input_connector_rects.push_back(connector_rect);
    y = connector_bottom_y;
  }

  return input_connector_rects;
}


static ViewportCircle connectorCircleInRect(const ViewportRect &connector_rect)
{
  ViewportCircle c;
  c.center = connector_rect.center();
  c.radius = connector_radius;
  return c;
}


static vector<ViewportRect>
  calculateOutputConnectorRects(
    const Node &node,
    const vector<ViewportRect> &line_rects,
    const float right_outer_x,
    const float connector_distance
  )
{
  vector<ViewportRect> output_connector_rects;
  {
    size_t statement_index = 0;
    size_t n_statements = node.statements.size();
    size_t line_index = 0;

    for (; statement_index != n_statements; ++statement_index) {
      const auto &statement = node.statements[statement_index];
      const auto statement_n_lines = statement.n_lines;

      if (statement.has_output) {
        float statement_start_y =
          line_rects[line_index + statement_n_lines - 1].start.y;
        float statement_end_y = line_rects[line_index].end.y;

        float connector_left_x = right_outer_x + connector_distance;
        float connector_right_x = connector_left_x + connector_radius*2;
        ViewportRect rect{
          {connector_left_x,statement_start_y},
          {connector_right_x,statement_end_y}
        };
        output_connector_rects.push_back(rect);
      }

      line_index += statement_n_lines;
    }
  }

  return output_connector_rects;
}


static vector<ViewportCircle>
  connectorCirclesInRects(const vector<ViewportRect> &rects)
{
  vector<ViewportCircle> circles;

  for (auto &rect : rects) {
    circles.push_back(connectorCircleInRect(rect));
  }

  return circles;
}


// text_rect: the bounding box of some text if it was positioned at 0,0,
// rect:  a rectangle that we want to fit the text inside.
// returns the correspnoding position of the text.
// That is if the text with bounding box text_rect is rendered at
// the returned position, it will fit inside the given rect.
static ViewportPoint
  textPosition(const ViewportRect &rect,const ViewportRect &text_rect)
{
  float extra_x = rect.size().x - text_rect.size().x;
  float extra_y = rect.size().y - text_rect.size().y;
  float offset_x = extra_x/2;
  float offset_y = extra_y/2;
  float adjusted_start_x = rect.start.x + offset_x;
  float adjusted_start_y = rect.start.y + offset_y;
  float pos_x = adjusted_start_x - text_rect.start.x;
  float pos_y = adjusted_start_y - text_rect.start.y;

  return ViewportPoint(pos_x,pos_y);
}


static NodeRenderInfo
  calculateNodeRenderInfo(
    const Node &node,
    const ViewportRect &header_rect,
    const vector<ViewportRect> &line_text_rects,
      // These rectangles show the bounding box of the line text if
      // the position was at 0,0.
    const ViewportRect &dollar_rect,
    const float connector_radius
  )
{
  const int n_inputs = node.nInputs();
  const float connector_distance = 5;
    // Distance between the connector circles and the node body.
  ViewportPoint top_left = nodeBodyTopLeft(header_rect);

  // We determine the line positions so that their rectangles stack
  // on top of each other.
  const vector<ViewportPoint> line_text_positions =
    calculateLineTextPositions(top_left,line_text_rects);

  vector<ViewportRect> line_rects =
    calculateLineRects(line_text_rects,line_text_positions);

  const float left_x = top_left.x;
  const float top_y = top_left.y;
  size_t n_lines = line_text_rects.size();

  const float margin = 5;
    // Horizontal distance between the text of the node lines and the
    // outer rectangle of the node body.

  const float left_outer_x = left_x - margin;

  float line_height = dollar_rect.size().y;

  vector<ViewportRect> input_connector_rects =
    calculateInputConnectorRects(
      left_outer_x,
      top_y,
      connector_distance,
      connector_radius,
      n_inputs,
      line_height
    );

  Optional<float> maybe_bottom_y;

  {
    // Layout the lines and the input connectors that go with each line.
    float line_top_y = top_y;
    int line_start_input = 0;

    for (size_t line_index=0; line_index!=n_lines; ++line_index) {
      float line_height = line_rects[line_index].size().y;
      float total_input_height = 0;
      int line_n_inputs = node.lines[line_index].n_inputs;

      for (int i=0; i!=line_n_inputs; ++i) {
        total_input_height +=
          input_connector_rects[line_start_input + i].size().y;
      }

      float height = std::max(line_height, total_input_height);
      float input_top_y = line_top_y - (height - total_input_height)/2;

      for (int i=0; i!=line_n_inputs; ++i) {
        float input_bottom_y =
          input_top_y - input_connector_rects[line_start_input + i].size().y;
        input_connector_rects[line_start_input + i].end.y = input_top_y;
        input_connector_rects[line_start_input + i].start.y = input_bottom_y;
        input_top_y = input_bottom_y;
      }

      float line_bottom_y = line_top_y - height;

      line_rects[line_index].end.y = line_top_y;
      line_rects[line_index].start.y = line_bottom_y;

      line_top_y = line_bottom_y;
      line_start_input += line_n_inputs;
    }

    maybe_bottom_y = line_top_y;
  }

  float &bottom_y = *maybe_bottom_y;

  vector<ViewportTextObject> text_objects;

  // Add text objects for each line.
  for (size_t line_index=0; line_index!=n_lines; ++line_index) {
    const auto &line_text = node.lines[line_index].text;
    float start_x = line_rects[line_index].start.x;
    float start_y = line_rects[line_index].start.y;
    float end_x = line_rects[line_index].end.x;
    float end_y = line_rects[line_index].end.y;
    ViewportPoint start{start_x, start_y};
    ViewportPoint end{end_x, end_y};
    ViewportRect rect{start,end};
    ViewportPoint aligned_position =
      textPosition(rect,line_text_rects[line_index]);

    text_objects.push_back(ViewportTextObject{line_text, aligned_position});
  }

  const float right_x = calculateRightX(header_rect,line_rects);
  float right_outer_x = right_x + margin;

  // Add output connector circles
  vector<ViewportRect> output_connector_rects =
    calculateOutputConnectorRects(
      node,
      line_rects,
      right_outer_x,
      connector_distance
    );

  vector<ViewportCircle> input_connector_circles =
    connectorCirclesInRects(input_connector_rects);

  vector<ViewportCircle> output_connector_circles =
    connectorCirclesInRects(output_connector_rects);

  ViewportRect body_rect;
  body_rect.start = ViewportPoint(left_x,bottom_y);
  body_rect.end = ViewportPoint(right_x,top_y);

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


NodeRenderInfo DiagramEditor::nodeRenderInfo(const Node &node) const
{
  const DiagramTextObject &header_text_object = node.header_text_object;

  ViewportRect header_rect = nodeHeaderRect(header_text_object);

  vector<ViewportRect> line_text_rects;

  for (const auto& text : node.lineTexts()) {
    line_text_rects.push_back(rectAroundText(text));
  }

  ViewportRect dollar_rect = rectAroundText("$");

  return
    calculateNodeRenderInfo(
      node,
      header_rect,
      line_text_rects,
      dollar_rect,
      connector_radius
    );
}


bool
  DiagramEditor::nodeContains(
    NodeIndex node_index,
    const ViewportPoint &p
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


NodeIndex DiagramEditor::indexOfNodeContaining(const ViewportPoint &p) const
{
  for (NodeIndex i : diagram().existingNodeIndices()) {
    if (nodeContains(i,p)) {
      return i;
    }
  }

  return noNodeIndex();
}


ViewportCircle DiagramEditor::nodeInputCircle(const Node &node,int input_index)
{
  NodeRenderInfo render_info = nodeRenderInfo(node);
  return render_info.input_connector_circles[input_index];
}


bool
  DiagramEditor::nodeInputContains(
    int node_index,
    int input_index,
    const ViewportPoint &p
  )
{
  return nodeInputCircle(node(node_index),input_index).contains(p);
}


ViewportCircle
  DiagramEditor::nodeOutputCircle(const Node &node,int output_index)
{
  NodeRenderInfo render_info = nodeRenderInfo(node);
  return render_info.output_connector_circles[output_index];
}


bool
  DiagramEditor::nodeOutputContains(
    int node_index,
    int output_index,
    const ViewportPoint &p
  )
{
  return nodeOutputCircle(node(node_index),output_index).contains(p);
}


NodeConnectorIndex
  DiagramEditor::indexOfNodeConnectorContaining(const ViewportPoint &p)
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
    const ViewportPoint &p
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
  const ViewportPoint &p;

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
    const ViewportPoint &p
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


void DiagramEditor::mouseReleasedAt(ViewportPoint mouse_release_position)
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
    ViewportPoint p,
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
    ViewportPoint p,
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


void DiagramEditor::mouseMovedTo(const ViewportPoint &mouse_position)
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
    const DiagramPoint &diagram_coords
  ) const -> ViewportPoint
{
  return ViewportPoint(Point2D(diagram_coords) + Vector2D(view_offset));
}


DiagramPoint
  DiagramEditor::diagramCoordsFromViewportCoords(
    const ViewportPoint &viewport_coords
  ) const
{
  return DiagramPoint(viewport_coords - view_offset);
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


auto DiagramEditor::maybeNodeLineAt(const ViewportPoint &p) const
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
  DiagramEditor::maybeToolTipTextAt(const ViewportPoint &p) const
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
