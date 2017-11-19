#include "qtdiagrameditor.hpp"

#include <iostream>

using std::cerr;
using std::vector;
using std::string;


static QString qString(const std::string &arg)
{
  return QString::fromUtf8(arg.c_str());
}


QtDiagramEditor::QtDiagramEditor()
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
    if (selected_node_index>=0) {
      deleteNode(selected_node_index);
      selected_node_index = -1;
      update();
      return;
    }

    bool a_node_is_focused = (focused_node_index >= 0);

    if (a_node_is_focused) {
      focusedText().erase(focusedText().end()-1);
    }
  }
  else if (key_event_ptr->key()==Qt::Key_Return) {
    enterPressed();
    return;
  }
  else {
    if (focused_node_index>=0) {
      focusedText() += key_event_ptr->text().toStdString();
    }
  }

  update();
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
  int n_nodes = nodes.size();

  for (int i=0; i!=n_nodes; ++i) {
    if (contains(nodes[i].text_object,p)) {
      return i;
    }
  }

  return -1;
}


int QtDiagramEditor::indexOfNode2Containing(const Point2D &p)
{
  int n_nodes = node2s.size();

  for (int i=0; i!=n_nodes; ++i) {
    if (contains(node2s[i].header_text_object,p)) {
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
  return defaultNodeInputCircle(node_index,input_index).contains(p);
}


NodeInputIndex QtDiagramEditor::indexOfNodeInputContaining(const Point2D &p)
{
  int n_text_objects = nodes.size();

  for (int i=0; i!=n_text_objects; ++i) {
    int n_inputs = nodes[i].inputs.size();
    for (int j=0; j!=n_inputs; ++j) {
      if (nodeInputContains(i,j,p)) {
        NodeInputIndex result;
        result.node_index = i;
        result.input_index = j;
        return result;
      }
    }
  }

  return NodeInputIndex::null();
}


void QtDiagramEditor::mousePressedAt(Point2D p)
{
  mouse_press_position = p;
  node_was_selected = false;

  if (focused_node_index>=0) {
    if (nodes[focused_node_index].text_object.text.empty()) {
      deleteNode(focused_node_index);
    }
    else {
      updateNodeInputs(focused_node_index);
    }
    focused_node_index = -1;
  }

  if (selected_node_index>=0) {
    if (nodes[selected_node_index].text_object.text.empty()) {
      deleteNode(selected_node_index);
      selected_node_index = -1;
    }
  }

  if (selected_node2_index>=0) {
    selected_node2_index = -1;
  }

  {
    NodeInputIndex i = indexOfNodeInputContaining(mouse_press_position);

    if (i!=NodeInputIndex::null()) {
      selected_node_input_index = i;
      selected_node_index = -1;
      temp_source_pos = mouse_press_position;
      update();
      return;
    }
  }

  {
    int i = indexOfNodeContaining(p);

    if (i>=0) {
      if (i==selected_node_index) {
        node_was_selected = true;
      }
      else {
        node_was_selected = false;
        selectNode(i);
      }
      original_node_position = nodes[i].text_object.position;
      update();
      return;
    }
  }

  selected_node_index = -1;
  selected_node_input_index = NodeInputIndex::null();

  {
    int i = indexOfNode2Containing(p);

    if (i>=0) {
      selected_node2_index = i;
      original_node_position = node2s[i].header_text_object.position;
      update();
      return;
    }
  }

  // Add a new node and focus it.
  {
    TextObject new_text_object;
    new_text_object.position = mouse_press_position;
    new_text_object.text = "";

    int new_node_index = addNode(new_text_object);
    focused_node_index = new_node_index;
  }

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
  if (!selected_node_input_index.isNull()) {
    int source_node_index = indexOfNodeContaining(temp_source_pos);
    nodes[selected_node_input_index.node_index].inputs[
      selected_node_input_index.input_index].source_node_index =
        source_node_index;
    selected_node_input_index.clear();
    update();
  }
  else if (node_was_selected) {
    focused_node_index = selected_node_index;
    selected_node_index = -1;
    update();
  }
}


void QtDiagramEditor::mouseMoveEvent(QMouseEvent * event_ptr)
{
  Point2D mouse_position = screenToGLCoords(event_ptr->x(),event_ptr->y());

  if (!selected_node_input_index.isNull()) {
    temp_source_pos = mouse_position;
    update();
    return;
  }

  if (selected_node_index>=0) {
    nodes[selected_node_index].text_object.position =
      original_node_position + (mouse_position - mouse_press_position);
    update();
    return;
  }

  if (selected_node2_index>=0) {
    node2s[selected_node2_index].header_text_object.position =
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


void QtDiagramEditor::drawPolygon(const std::vector<Point2D> &vertices)
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

    color_data[i*color_size + 0] = 0.5;
    color_data[i*color_size + 1] = 0.5;
    color_data[i*color_size + 2] = 0;
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


void QtDiagramEditor::drawCircle(const Circle &circle)
{
  drawClosedLine(verticesOf(circle));
}


void QtDiagramEditor::drawFilledRect(const Rect &rect)
{
  drawPolygon(verticesOf(rect));
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


Rect QtDiagramEditor::nodeRect(const TextObject &text_object)
{
  return withMargin(rectAroundText(text_object),5);
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
  QtDiagramEditor::drawBoxedText(
    const TextObject &text_object,
    bool is_selected
  )
{
  Rect rect = nodeRect(text_object);

  if (is_selected) {
    drawFilledRect(rect);
  }

  drawRect(rect);
  drawText(text_object);
}


int QtDiagramEditor::textHeight() const
{
  return fontMetrics().height();
}


int QtDiagramEditor::textWidth(const std::string &s) const
{
  return fontMetrics().width(qString(s));
}


void QtDiagramEditor::drawCursor(const TextObject &text_object)
{
  float cursor_height = textHeight();
  float text_width = textWidth(text_object.text);
  float descent = fontMetrics().descent();
  Point2D p = text_object.position + Point2D{text_width,-descent};
  drawLine(p,p+Point2D{0,cursor_height});
}


Point2D
  QtDiagramEditor::defaultNodeInputPosition(
    int node_index,
    int input_index
  )
{
  const TextObject &text_object = nodes[node_index].text_object;
  float radius = node_input_radius;
  float spacing = 5;
  float upper_left_x = nodeRect(text_object).start.x;
  float upper_left_y = nodeRect(text_object).end.y;
  float x = upper_left_x - radius*2;
  float y = upper_left_y - ((radius*2 + spacing) * input_index + spacing);

  return Point2D{x,y};
}


Circle QtDiagramEditor::defaultNodeInputCircle(int node_index,int input_index)
{
  Circle circle;
  circle.center = defaultNodeInputPosition(node_index,input_index);
  circle.radius = node_input_radius;

  return circle;
}


Point2D QtDiagramEditor::nodeOutputPosition(int node_index)
{
  Rect rect = nodeRect(nodes[node_index].text_object);
  float x = rect.end.x;
  float y = (rect.start.y + rect.end.y)/2;
  return Point2D{x,y};
}


void QtDiagramEditor::drawNodeInput(int node_index,int input_index)
{
  NodeInputIndex node_input_index;
  node_input_index.node_index = node_index;
  node_input_index.input_index = input_index;
  const Node::Input &input = nodes[node_index].inputs[input_index];

  if (!input.name.empty()) {
    float horizontal_alignment = 1.0; // right side
    float vertical_alignment = 0.5; // centered
    Point2D position = defaultNodeInputPosition(node_index,input_index);
    drawAlignedText(
      input.name,position,horizontal_alignment,vertical_alignment
    );
  }

  int source_node_index = input.source_node_index;

  if (source_node_index>=0) {
    drawLine(
      nodeOutputPosition(source_node_index),
      defaultNodeInputPosition(node_index,input_index)
    );
    return;
  }

  Circle circle = defaultNodeInputCircle(node_index,input_index);

  if (node_input_index==selected_node_input_index) {
    circle.center = temp_source_pos;
  }

  drawCircle(circle);

  if (node_input_index==selected_node_input_index) {
    drawFilledCircle(circle);
  }

  drawLine(circle.center,defaultNodeInputPosition(node_index,input_index));
}


void QtDiagramEditor::drawNodeInputs(int node_index)
{
  int n_inputs = nodes[node_index].inputs.size();

  for (int i=0; i!=n_inputs; ++i) {
    drawNodeInput(node_index,i);
  }
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

template <typename T>
static vector<T> operator+(const vector<T> &a,const vector<T> &b)
{
  vector<T> result;
  result.insert(result.end(),a.begin(),a.end());
  result.insert(result.end(),b.begin(),b.end());
  return result;
}


void QtDiagramEditor::drawNode2(const Node2 &node,bool is_selected)
{
  const TextObject &header_text_object = node.header_text_object;
  drawBoxedText(header_text_object,is_selected);

  // We need to determine a rectangle that fits around the contents.
  Rect header_rect = nodeRect(header_text_object);

  // The top of the rectangle should be the bottom of the header text
  // object.
  float top_y = header_rect.start.y;

  // The left side should be the left of the header rect
  float left_x = header_rect.start.x;

  // Start with the bottom being at the top.
  // For each string, we determine its rectangle, and then move the bottom
  // to the bottom of that rectangle.
  float bottom_y = top_y;

  const vector<string> &inputs = node.inputs;
  const vector<string> &outputs = node.outputs;
  vector<string> strings = inputs + outputs;

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

  for (const auto& s : strings) {
    Rect r =
      rectAroundText(alignedTextObject(
        s,
        Point2D(left_x,top_y),
        /*horizontal_alignment*/0,
        /*vertical_alignment*/1
      ));
    if (r.end.x > right_x) {
      right_x = r.end.x;
    }
  }

  // Draw the rectangle around all the inputs and outputs.
  {
    Rect r;
    r.start = Point2D(left_x,bottom_y);
    r.end   = Point2D(right_x,top_y);
    drawRect(r);
  }

  float y = top_y;
  float connector_radius = 5;

  // Draw the inputs

  for (const auto &s : inputs) {
    TextObject t = inputTextObject(s,left_x,y);
    drawText(t);
    Rect r = rectAroundText(t);

    float connector_x = (left_x - connector_radius - 5);
    float connector_y = (r.start.y + r.end.y)/2;

    Circle c;
    c.center = Point2D(connector_x,connector_y);
    c.radius = connector_radius;
    drawCircle(c);

    y = r.start.y;
  }

  // Draw the outputs

  for (const auto &s : outputs) {
    TextObject t = outputTextObject(s,right_x,y);
    drawText(t);
    Rect r = rectAroundText(t);

    float connector_x = (right_x + connector_radius + 5);
    float connector_y = (r.start.y + r.end.y)/2;

    Circle c;
    c.center = Point2D(connector_x,connector_y);
    c.radius = connector_radius;
    drawCircle(c);

    y = r.start.y;
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

  if (selected_node_index<0 && selected_node_input_index.isNull()) {
    if (focused_node_index>=0) {
      drawCursor(nodes[focused_node_index].text_object);
    }
  }

  {
    int n_text_objects = nodes.size();

    for (int index=0; index!=n_text_objects; ++index) {
      auto& text_object = nodes[index].text_object;
      bool is_selected = (selected_node_index==index);
      drawBoxedText(text_object,is_selected);
      drawNodeInputs(index);
    }
  }

  {
    int n_nodes = node2s.size();

    for (int index=0; index!=n_nodes; ++index) {
      const auto &node = node2s[index];
      bool is_selected = (selected_node2_index == index);
      drawNode2(node,is_selected);
    }
  }
}


void QtDiagramEditor::addTestNode()
{
  node2s.emplace_back();
  Node2 &node = node2s.back();
  node.header_text_object.text = "Add";
  node.header_text_object.position = Point2D(100,200);
  node.inputs = vector<string>{"a","b"};
  node.outputs = {"a+b"};
}
