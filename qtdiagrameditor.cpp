#include "qtdiagrameditor.hpp"

#include <iostream>

using std::cerr;
using std::vector;
using std::string;


static QString qString(const std::string &arg)
{
  return QString::fromUtf8(arg.c_str());
}


QtDiagramEditor::QtDiagramEditor(Diagram &diagram)
: DiagramEditor(diagram)
{
  {
    QFont font;
    font.setPointSize(20);
    setFont(font);
  }
  setFocusPolicy(Qt::StrongFocus);
}


void QtDiagramEditor::keyPressEvent(QKeyEvent *key_event_ptr)
{
  assert(key_event_ptr);

  if (key_event_ptr->key()==Qt::Key_Backspace) {
    backspacePressed();
    return;
  }
  if (key_event_ptr->key()==Qt::Key_Return) {
    enterPressed();
    return;
  }
  if (key_event_ptr->key()==Qt::Key_Up) {
    node_editor.text_editor.up();
    update();
    return;
  }
  if (key_event_ptr->key()==Qt::Key_Down) {
    if (node_editor.aNodeIsFocused()) {
      node_editor.text_editor.down();
      update();
    }
    return;
  }
  if (key_event_ptr->key()==Qt::Key_Left) {
    if (node_editor.aNodeIsFocused()) {
      node_editor.text_editor.left();
      update();
    }
    return;
  }
  if (key_event_ptr->key()==Qt::Key_Right) {
    if (node_editor.aNodeIsFocused()) {
      node_editor.text_editor.right();
      update();
    }
    return;
  }

  std::string new_text = key_event_ptr->text().toStdString();

  if (new_text!="") {
    textTyped(new_text);
    return;
  }
}


Point2D QtDiagramEditor::screenToGLCoords(int x,int y) const
{
  return Point2D{static_cast<float>(x),height()-static_cast<float>(y)};
}


bool QtDiagramEditor::contains(const TextObject &text_object,const Point2D &p)
{
  return nodeRect(text_object).contains(p);
}


int QtDiagramEditor::indexOfNodeContaining(const Point2D &p)
{
  for (NodeIndex i : diagram.existingNodeIndices()) {
    Node &node = diagram.node(i);
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


bool
  QtDiagramEditor::nodeInputContains(
    int node_index,
    int input_index,
    const Point2D &p
  )
{
  return nodeInputCircle(node(node_index),input_index).contains(p);
}


bool
  QtDiagramEditor::nodeOutputContains(
    int node_index,
    int output_index,
    const Point2D &p
  )
{
  return nodeOutputCircle(node(node_index),output_index).contains(p);
}


NodeConnectorIndex
  QtDiagramEditor::indexOfNodeConnectorContaining(const Point2D &p)
{
  for (NodeIndex i : diagram.existingNodeIndices()) {
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


void QtDiagramEditor::mousePressedAt(Point2D p)
{
  mouse_press_position = p;
  node_editor.node_was_selected = false;

  if (node_editor.aNodeIsFocused()) {
    if (node_editor.focusedNode(diagram).isEmpty()) {
      deleteNode(node_editor.focused_node_index);
    }
  }

  if (node_editor.aNodeIsFocused()) {
    unfocus();
  }

  {
    int i = indexOfNodeContaining(p);

    if (i>=0) {
      node_editor.node_was_selected = (i==node_editor.selected_node_index);
      node_editor.selected_node_index = i;
      node_editor.focused_node_index = -1;
      original_node_position = node(i).header_text_object.position;
      update();
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
      update();
      return;
    }
  }

  int new_node_index = addNode("",mouse_press_position);
  node_editor.focusNode(new_node_index,diagram);

  update();
}


void QtDiagramEditor::mousePressEvent(QMouseEvent *event_ptr)
{
  assert(event_ptr);
  Point2D p = screenToGLCoords(event_ptr->x(),event_ptr->y());
  mousePressedAt(p);
}


void QtDiagramEditor::mouseReleaseEvent(QMouseEvent *)
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
    update();
    return;
  }
  if (node_editor.node_was_selected) {
    node_editor.focusNode(node_editor.selected_node_index,diagram);
    node_editor.selected_node_index = -1;
    update();
    return;
  }
}


void QtDiagramEditor::mouseMoveEvent(QMouseEvent * event_ptr)
{
  Point2D mouse_position = screenToGLCoords(event_ptr->x(),event_ptr->y());

  if (!selected_node_connector_index.isNull()) {
    temp_source_pos = mouse_position;
    update();
    return;
  }

  if (node_editor.selected_node_index>=0) {
    node(node_editor.selected_node_index).header_text_object.position =
      original_node_position + (mouse_position - mouse_press_position);
    update();
    return;
  }
}


void QtDiagramEditor::drawLine(Point2D line_start,Point2D line_end)
{
  float vertices[][2] = {
    {line_start.x,line_start.y},
    {line_end.x,line_end.y},
  };

  float colors[][3] = {
    {1,1,1},
    {1,1,1}
  };

  glVertexPointer(/*size*/2,/*type*/GL_FLOAT,/*stride*/0,vertices);
  glColorPointer(/*size*/3,/*type*/GL_FLOAT,/*stride*/0,colors);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  {
    glColor3f(1,1,1);
    unsigned int line_indices[] = {0,1};
    GLenum mode = GL_LINES;
    GLsizei count = 2;
    GLenum type = GL_UNSIGNED_INT;

    glDrawElements(mode,count,type,line_indices);
  }

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}


void
  QtDiagramEditor::setupProjection(float viewport_width,float viewport_height)
{
  GLdouble left = 0;
  GLdouble right = viewport_width;
  GLdouble bottom = 0;
  GLdouble top = viewport_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(left,right,bottom,top);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void QtDiagramEditor::drawClosedLine(const std::vector<Point2D> &vertices)
{
  int n = vertices.size();

  for (int i=0; i!=n; ++i) {
    drawLine(vertices[i],vertices[(i+1)%n]);
  }
}


void QtDiagramEditor::drawPolygon(const std::vector<Point2D> &vertices,const Color &color)
{
  int n_vertices = vertices.size();
  int vertex_size = 2;
  int color_size = 3;
  std::vector<float> vertex_data;
  vertex_data.resize(n_vertices*vertex_size);
  std::vector<float> color_data;
  color_data.resize(n_vertices*color_size);

  for (int i=0; i!=n_vertices; ++i) {
    vertex_data[i*vertex_size + 0] = vertices[i].x;
    vertex_data[i*vertex_size + 1] = vertices[i].y;

    color_data[i*color_size + 0] = color.r;
    color_data[i*color_size + 1] = color.g;
    color_data[i*color_size + 2] = color.b;
  }

  glVertexPointer(
    vertex_size,/*type*/GL_FLOAT,/*stride*/0,vertex_data.data()
  );
  glColorPointer(
    color_size,/*type*/GL_FLOAT,/*stride*/0,color_data.data()
  );

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  {
    std::vector<unsigned int> index_data(n_vertices);
    for (int i=0; i!=n_vertices; ++i) {
      index_data[i] = i;
    }
    GLsizei count = n_vertices;
    GLenum type = GL_UNSIGNED_INT;

    glDrawElements(GL_POLYGON,count,type,index_data.data());
  }

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}


void QtDiagramEditor::drawPolygon(const std::vector<Point2D> &vertices)
{
  drawPolygon(vertices,Color{0.5,0.5,0});
}


std::vector<Point2D> QtDiagramEditor::verticesOf(const Rect &rect)
{
  std::vector<Point2D> vertices;

  float x1 = rect.start.x;
  float y1 = rect.start.y;
  float x2 = rect.end.x;
  float y2 = rect.end.y;

  vertices.push_back(Point2D{x1,y1});
  vertices.push_back(Point2D{x2,y1});
  vertices.push_back(Point2D{x2,y2});
  vertices.push_back(Point2D{x1,y2});

  return vertices;
}


std::vector<Point2D>
  QtDiagramEditor::roundedVerticesOf(const Rect &rect,float offset)
{
  std::vector<Point2D> vertices;
  float radius = 5;
  float v = radius*sqrtf(2)/2;

  float x1 = rect.start.x - offset;
  float x1a = x1 + radius;
  float x1b = x1a - v;
  float y1 = rect.start.y - offset;
  float y1a = y1 + radius;
  float y1b = y1a - v;
  float x2 = rect.end.x + offset;
  float x2a = x2 - radius;
  float x2b = x2a + v;
  float y2 = rect.end.y + offset;
  float y2a = y2 - radius;
  float y2b = y2a + v;

  vertices.push_back(Point2D{x1,y1a});
  vertices.push_back(Point2D{x1b,y1b});
  vertices.push_back(Point2D{x1a,y1});

  vertices.push_back(Point2D{x2a,y1});
  vertices.push_back(Point2D{x2b,y1b});
  vertices.push_back(Point2D{x2,y1a});

  vertices.push_back(Point2D{x2,y2a});
  vertices.push_back(Point2D{x2b,y2b});
  vertices.push_back(Point2D{x2a,y2});

  vertices.push_back(Point2D{x1a,y2});
  vertices.push_back(Point2D{x1b,y2b});
  vertices.push_back(Point2D{x1,y2a});

  return vertices;
}




std::vector<Point2D> QtDiagramEditor::verticesOf(const Circle &circle)
{
  Point2D center = circle.center;
  float radius = circle.radius;
  std::vector<Point2D> vertices;

  for (int i=0; i!=10; ++i) {
    float fraction = i/10.0;
    float angle = 2*M_PI * fraction;
    float x = center.x + cos(angle)*radius;
    float y = center.y + sin(angle)*radius;
    vertices.push_back(Point2D{x,y});
  }

  return vertices;
}


void QtDiagramEditor::drawRect(const Rect &arg)
{
  drawClosedLine(verticesOf(arg));
}


void QtDiagramEditor::drawRoundedRect(const Rect &arg)
{
  float offset = 0.5;
  drawClosedLine(roundedVerticesOf(arg,offset));
}


void QtDiagramEditor::drawCircle(const Circle &circle)
{
  drawClosedLine(verticesOf(circle));
}


void QtDiagramEditor::drawFilledRect(const Rect &rect)
{
  drawPolygon(verticesOf(rect));
}


void QtDiagramEditor::drawFilledRoundedRect(const Rect &rect,const Color &color)
{
  float offset = 0;
  drawPolygon(roundedVerticesOf(rect,offset),color);
}


void QtDiagramEditor::drawFilledCircle(const Circle &circle)
{
  drawPolygon(verticesOf(circle));
}


Rect QtDiagramEditor::rectAroundText(const TextObject &text_object) const
{
  std::string text = text_object.text;
  if (text == "") {
    // We want to avoid the box collapsing down to nothing.
    text = " ";
  }
  auto position = text_object.position;

  QFontMetrics fm = fontMetrics();
  auto rect = fm.boundingRect(qString(text));

  auto tl = rect.topLeft();
  auto br = rect.bottomRight();
  auto x = position.x;
  auto y = position.y;
  auto bx = x+tl.x();
  auto ex = x+br.x()+1;
  auto by = y-br.y()-1;
  auto ey = y-tl.y();
  auto begin = Point2D{bx,by};
  auto end =   Point2D{ex,ey};
  return Rect{begin,end};
}


Rect QtDiagramEditor::nodeRect(const TextObject &text_object) const
{
  return withMargin(rectAroundText(text_object),5);
}


Rect QtDiagramEditor::nodeHeaderRect(const TextObject &text_object) const
{
  if (text_object.text=="") {
    Point2D start = text_object.position;
    Point2D end = start;
    return Rect{start,end};
  }
  return nodeRect(text_object);
}


Point2D
  QtDiagramEditor::alignmentPoint(
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
  QtDiagramEditor::alignedTextObject(
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


void
  QtDiagramEditor::drawAlignedText(
    const std::string &text,
    const Point2D &position,
    float horizontal_alignment,
    float vertical_alignment
  )
{
  TextObject text_object =
    alignedTextObject(text,position,horizontal_alignment,vertical_alignment);
  drawText(text_object);
}


void QtDiagramEditor::drawText(const TextObject &text_object)
{
  Point2D position = text_object.position;
  renderText(position.x,position.y,0,qString(text_object.text));
}


void
  QtDiagramEditor::drawBoxedText2(
    const TextObject &text_object,
    bool is_selected,
    const Rect &rect
  )
{
  if (is_selected) {
    drawFilledRect(rect);
  }

  drawRect(rect);
  drawText(text_object);
}


void
  QtDiagramEditor::drawBoxedText(
    const TextObject &text_object,
    bool is_selected
  )
{
  Rect rect = nodeRect(text_object);
  drawBoxedText2(text_object,is_selected,rect);
}


int QtDiagramEditor::textHeight() const
{
  return fontMetrics().height();
}


int QtDiagramEditor::textWidth(const std::string &s) const
{
  return fontMetrics().width(qString(s));
}


void QtDiagramEditor::drawCursor(const TextObject &text_object,int column_index)
{
  float cursor_height = textHeight();
  float text_width = textWidth(text_object.text.substr(0,column_index));
  float descent = fontMetrics().descent();
  Point2D p = text_object.position + Point2D{text_width,-descent};
  drawLine(p,p+Point2D{0,cursor_height});
}


void QtDiagramEditor::drawCursor(const TextObject &text_object)
{
  drawCursor(text_object,text_object.text.length());
}


template <typename T>
static vector<T> operator+(const vector<T> &a,const vector<T> &b)
{
  vector<T> result;
  result.insert(result.end(),a.begin(),a.end());
  result.insert(result.end(),b.begin(),b.end());
  return result;
}


Circle QtDiagramEditor::nodeInputCircle(const Node &node,int input_index)
{
  NodeRenderInfo render_info = nodeRenderInfo(node);
  return render_info.input_connector_circles[input_index];
}


Circle QtDiagramEditor::nodeOutputCircle(const Node &node,int output_index)
{
  NodeRenderInfo render_info = nodeRenderInfo(node);
  return render_info.output_connector_circles[output_index];
}


Circle QtDiagramEditor::connectorCircle(NodeConnectorIndex index) const
{
  NodeRenderInfo render_info = nodeRenderInfo(node(index.node_index));

  if (index.input_index>=0) {
    return render_info.input_connector_circles[index.input_index];
  }

  if (index.output_index>=0) {
    return render_info.output_connector_circles[index.output_index];
  }

  assert(false);
  return Circle{};
}



TextObject
  QtDiagramEditor::inputTextObject(const string &s,float left_x,float y) const
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


TextObject
  QtDiagramEditor::outputTextObject(const string &s,float right_x,float y) const
{
  TextObject t =
    alignedTextObject(
      s,
      Point2D(right_x,y),
      /*horizontal_alignment*/1,
      /*vertical_alignment*/1
    );
  return t;
}

Rect
  QtDiagramEditor::nodeBodyRect(
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


NodeRenderInfo QtDiagramEditor::nodeRenderInfo(const Node &node) const
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

  float y = top_y;

  for (const auto &line : node.lines) {
    TextObject t = inputTextObject(line.text,left_x,y);
    Rect r = rectAroundText(t);
    render_info.text_objects.push_back(t);
    if (line.has_input) {
      float connector_x = (left_outer_x - connector_radius - 5);
      float connector_y = (r.start.y + r.end.y)/2;

      Circle c;
      c.center = Point2D(connector_x,connector_y);
      c.radius = connector_radius;
      render_info.input_connector_circles.push_back(c);

    }
    if (line.has_output) {
      float connector_x = (right_outer_x + connector_radius + 5);
      float connector_y = (r.start.y + r.end.y)/2;

      Circle c;
      c.center = Point2D(connector_x,connector_y);
      c.radius = connector_radius;
      render_info.output_connector_circles.push_back(c);
    }
    y = r.start.y;
  }

  render_info.body_outer_rect = body_rect;
  render_info.body_outer_rect.start.x = left_outer_x;
  render_info.body_outer_rect.end.x = right_outer_x;

  return render_info;
}


void QtDiagramEditor::drawNode(NodeIndex node_index)
{
  const Node &node = this->node(node_index);
  NodeRenderInfo render_info = nodeRenderInfo(node);

  bool is_selected = (node_editor.selected_node_index == node_index);
  const TextObject &header_text_object = node.header_text_object;
  drawBoxedText2(header_text_object,is_selected,render_info.header_rect);

  Color unselected_color{0.25,0.25,0.5};
  Color selected_color{0.5,0.5,0};
  // Draw the rectangle around all the inputs and outputs.
  if (is_selected) {
    drawFilledRoundedRect(render_info.body_outer_rect,selected_color);
  }
  else {
    drawFilledRoundedRect(render_info.body_outer_rect,unselected_color);
  }
  drawRoundedRect(render_info.body_outer_rect);

  // Draw the input labels

  int n_inputs = node.nInputs();
  int n_outputs = node.nOutputs();

  for (const auto &t : render_info.text_objects) {
    drawText(t);
  }

  // Draw the input connectors

  for (int i=0; i!=n_inputs; ++i) {
    Circle c = render_info.input_connector_circles[i];
    drawCircle(c);

    if (node_index==selected_node_connector_index.node_index &&
        i==selected_node_connector_index.input_index) {
      drawFilledCircle(c);
    }

    const Node::Input &input = node.inputs[i];

    if (input.source_node_index>=0) {
      const Node& source_node = this->node(input.source_node_index);
      int source_output_index = input.source_output_index;
      Circle source_circle = nodeOutputCircle(source_node,source_output_index);
      drawLine(source_circle.center,c.center);
    }
  }

  // Draw the output connectors

  for (int i=0; i!=n_outputs; ++i) {
    Circle c = render_info.output_connector_circles[i];
    drawCircle(c);
    if (node_index==selected_node_connector_index.node_index &&
        i==selected_node_connector_index.output_index) {
      drawFilledCircle(c);
    }
  }
}


void QtDiagramEditor::paintGL()
{
  GLint x = 0;
  GLint y = 0;
  GLsizei viewport_width = this->width();
  GLsizei viewport_height = this->height();

  glViewport(x,y,viewport_width,viewport_height);

  setupProjection(viewport_width,viewport_height);

  GLfloat red = 0;
  GLfloat green = 0;
  GLfloat blue = 0;
  GLfloat alpha = 1;

  glClearColor(red,green,blue,alpha);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (NodeIndex index : diagram.existingNodeIndices()) {
    drawNode(index);
  }

  if (node_editor.focused_node_index>=0) {
    NodeRenderInfo render_info =
      nodeRenderInfo(node(node_editor.focused_node_index));
    int line_index = node_editor.text_editor.cursorLineIndex();
    int column_index = node_editor.text_editor.cursorColumnIndex();
    drawCursor(render_info.text_objects[line_index],column_index);
  }

  if (!selected_node_connector_index.isNull()) {
    drawLine(
      connectorCircle(selected_node_connector_index).center,
      temp_source_pos
    );
  }

  diagram.evaluate(cerr);
}
