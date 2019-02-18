#include "qtdiagrameditor.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QToolTip>
#include "evaluatediagram.hpp"
#include "diagramio.hpp"
#include "qtmenu.hpp"
#include "draw.hpp"
#include "fakeexecutor.hpp"


using std::cerr;
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostream;


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



string QtDiagramEditor::askForSavePath()
{
  QFileDialog file_dialog;
  QString result =
    file_dialog.getSaveFileName(this,"Export Diagram","diagrams/diagram.dat");
  string path = result.toStdString();
  return path;
}


void QtDiagramEditor::showError(const string &message)
{
  QMessageBox box;
  box.setText(QString::fromStdString(message));
  box.exec();
}


void QtDiagramEditor::exportDiagramSlot()
{
  exportDiagramPressed();
}


string QtDiagramEditor::askForOpenPath()
{
  QFileDialog file_dialog;
  QString result =
    file_dialog.getOpenFileName(this,"Import Diagram","diagrams/diagram.dat");
  string path = result.toStdString();
  return path;
}


void QtDiagramEditor::importDiagramSlot()
{
  importDiagramPressed();
}


void QtDiagramEditor::keyPressEvent(QKeyEvent *key_event_ptr)
{
  assert(key_event_ptr);

  if (key_event_ptr->key()==Qt::Key_Backspace) {
    backspacePressed();
    return;
  }

  if (key_event_ptr->key()==Qt::Key_Delete) {
    deletePressed();
    return;
  }

  if (key_event_ptr->key()==Qt::Key_Return) {
    enterPressed();
    return;
  }

  if (key_event_ptr->key()==Qt::Key_Escape) {
    escapePressed();
    return;
  }

  if (key_event_ptr->key()==Qt::Key_Up) {
    text_editor.up();
    update();
    return;
  }

  if (key_event_ptr->key()==Qt::Key_Down) {
    if (aNodeIsFocused()) {
      text_editor.down();
      update();
    }
    return;
  }

  if (key_event_ptr->key()==Qt::Key_Left) {
    if (aNodeIsFocused()) {
      text_editor.left();
      update();
    }
    return;
  }

  if (key_event_ptr->key()==Qt::Key_Right) {
    if (aNodeIsFocused()) {
      text_editor.right();
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


auto QtDiagramEditor::screenToViewportCoords(int x,int y) const
  -> ViewportCoords
{
  return ViewportCoords{static_cast<float>(x),height()-static_cast<float>(y)};
}


bool
  QtDiagramEditor::contains(
    const DiagramTextObject &text_object,
    const ViewportCoords &p
  )
{
  return nodeRect(text_object).contains(p);
}


void QtDiagramEditor::mousePressEvent(QMouseEvent *event_ptr)
{
  assert(event_ptr);
  QMouseEvent &event = *event_ptr;
  ViewportCoords p = screenToViewportCoords(event.x(),event.y());

  if (event.button()==Qt::LeftButton) {
    Qt::KeyboardModifiers qt_modifiers = event.modifiers();
    EventModifiers modifiers;

    modifiers.shift_is_pressed = qt_modifiers.testFlag(Qt::ShiftModifier);
    modifiers.ctrl_is_pressed = qt_modifiers.testFlag(Qt::ControlModifier);

    leftMousePressedAt(p,modifiers);
  }
  else if (event.button()==Qt::RightButton) {
    QMenu menu;
    {
      QAction &action = createAction(menu,"Import Diagram...");
      connect(
        &action,
        SIGNAL(triggered()),
        SLOT(importDiagramSlot())
      );
    }
    {
      QAction &action = createAction(menu,"Export Diagram...");
      connect(
        &action,
        SIGNAL(triggered()),
        SLOT(exportDiagramSlot())
      );
    }
    menu.exec(mapToGlobal(event.pos()));
  }
  else if (event.button()==Qt::MiddleButton) {
    bool alt_is_pressed = event.modifiers().testFlag(Qt::AltModifier);
    EventModifiers modifiers;
    modifiers.alt_is_pressed = alt_is_pressed;
    middleMousePressedAt(p,modifiers);
  }
}


void QtDiagramEditor::mouseReleaseEvent(QMouseEvent *event_ptr)
{
  assert(event_ptr);
  ViewportCoords mouse_release_position =
    screenToViewportCoords(event_ptr->x(),event_ptr->y());

  mouseReleasedAt(mouse_release_position);
}


void QtDiagramEditor::mouseMoveEvent(QMouseEvent * event_ptr)
{
  ViewportCoords mouse_position =
    screenToViewportCoords(event_ptr->x(),event_ptr->y());

  mouseMovedTo(mouse_position);
}


void
  QtDiagramEditor::drawClosedLine(
    const std::vector<ViewportCoords> &vertices
  )
{
  int n = vertices.size();

  for (int i=0; i!=n; ++i) {
    drawLine(vertices[i],vertices[(i+1)%n]);
  }
}


void
  QtDiagramEditor::drawPolygon(
    const std::vector<ViewportCoords> &vertices,
    const Color &color
  )
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


void QtDiagramEditor::drawPolygon(const std::vector<ViewportCoords> &vertices)
{
  drawPolygon(vertices,Color{0.5,0.5,0});
}


std::vector<ViewportCoords>
  QtDiagramEditor::verticesOf(const ViewportRect &rect)
{
  std::vector<ViewportCoords> vertices;

  float x1 = rect.start.x;
  float y1 = rect.start.y;
  float x2 = rect.end.x;
  float y2 = rect.end.y;

  vertices.push_back(ViewportCoords{x1,y1});
  vertices.push_back(ViewportCoords{x2,y1});
  vertices.push_back(ViewportCoords{x2,y2});
  vertices.push_back(ViewportCoords{x1,y2});

  return vertices;
}


std::vector<ViewportCoords>
  QtDiagramEditor::roundedVerticesOf(const ViewportRect &rect,float offset)
{
  std::vector<ViewportCoords> vertices;
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

  vertices.push_back(ViewportCoords{x1,y1a});
  vertices.push_back(ViewportCoords{x1b,y1b});
  vertices.push_back(ViewportCoords{x1a,y1});

  vertices.push_back(ViewportCoords{x2a,y1});
  vertices.push_back(ViewportCoords{x2b,y1b});
  vertices.push_back(ViewportCoords{x2,y1a});

  vertices.push_back(ViewportCoords{x2,y2a});
  vertices.push_back(ViewportCoords{x2b,y2b});
  vertices.push_back(ViewportCoords{x2a,y2});

  vertices.push_back(ViewportCoords{x1a,y2});
  vertices.push_back(ViewportCoords{x1b,y2b});
  vertices.push_back(ViewportCoords{x1,y2a});

  return vertices;
}




std::vector<ViewportCoords> QtDiagramEditor::verticesOf(const Circle &circle)
{
  Point2D center = circle.center;
  float radius = circle.radius;
  std::vector<ViewportCoords> vertices;

  for (int i=0; i!=10; ++i) {
    float fraction = i/10.0;
    float angle = 2*M_PI * fraction;
    float x = center.x + cos(angle)*radius;
    float y = center.y + sin(angle)*radius;
    vertices.push_back(ViewportCoords{x,y});
  }

  return vertices;
}


void QtDiagramEditor::drawRect(const ViewportRect &arg)
{
  drawClosedLine(verticesOf(arg));
}


void QtDiagramEditor::drawRoundedRect(const ViewportRect &arg)
{
  float offset = 0.5;
  drawClosedLine(roundedVerticesOf(arg,offset));
}


void QtDiagramEditor::drawCircle(const Circle &circle)
{
  drawClosedLine(verticesOf(circle));
}


void QtDiagramEditor::drawFilledRect(const ViewportRect &rect)
{
  drawPolygon(verticesOf(rect));
}


void
  QtDiagramEditor::drawFilledRoundedRect(
    const ViewportRect &rect,const Color &color
  )
{
  float offset = 0;
  drawPolygon(roundedVerticesOf(rect,offset),color);
}


void QtDiagramEditor::drawFilledCircle(const Circle &circle)
{
  drawPolygon(verticesOf(circle));
}


ViewportLine
  QtDiagramEditor::textObjectCursorLine(
    const ViewportTextObject &text_object,
    int column_index
  ) const
{
  float cursor_height = textHeight();
  float text_width = textWidth(text_object.text.substr(0,column_index));
  float descent = fontMetrics().descent();
  ViewportCoords p = text_object.position + Vector2D{text_width,-descent};
  ViewportLine cursor_line{ p, p+Vector2D{0,cursor_height} };
  return cursor_line;
}


ViewportRect
  QtDiagramEditor::rectAroundText(const ViewportTextObject &text_object) const
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
  auto begin = ViewportCoords{bx,by};
  auto end =   ViewportCoords{ex,ey};

  return {begin,end};
}


void
  QtDiagramEditor::drawAlignedText(
    const std::string &text,
    const ViewportCoords &position,
    float horizontal_alignment,
    float vertical_alignment
  )
{
  ViewportTextObject text_object =
    alignedTextObject(text,position,horizontal_alignment,vertical_alignment);
  drawText(text_object);
}


void QtDiagramEditor::drawText(const ViewportTextObject &text_object)
{
  ViewportCoords position = text_object.position;
  renderText(position.x,position.y,0,qString(text_object.text));
}


void
  QtDiagramEditor::drawBoxedText2(
    const ViewportTextObject &text_object,
    bool is_selected,
    const ViewportRect &rect
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
    const DiagramTextObject &text_object,
    bool is_selected
  )
{
  ViewportRect rect = nodeRect(text_object);
  drawBoxedText2(viewportTextObject(text_object),is_selected,rect);
}


int QtDiagramEditor::textHeight() const
{
  return fontMetrics().height();
}


int QtDiagramEditor::textWidth(const std::string &s) const
{
  return fontMetrics().width(qString(s));
}



template <typename T>
static vector<T> operator+(const vector<T> &a,const vector<T> &b)
{
  vector<T> result;
  result.insert(result.end(),a.begin(),a.end());
  result.insert(result.end(),b.begin(),b.end());
  return result;
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


void QtDiagramEditor::drawNode(NodeIndex node_index)
{
  const Node &node = this->node(node_index);
  NodeRenderInfo render_info = nodeRenderInfo(node);

  bool is_selected = nodeIsSelected(node_index);

  { // Draw the header
    const DiagramTextObject &header_text_object = node.header_text_object;
    drawBoxedText2(
      viewportTextObject(header_text_object),
      is_selected,
      render_info.header_rect
    );
  }

  Color unselected_color{0.25,0.25,0.5};
  Color selected_color{0.5,0.5,0};

  Color fill_color = is_selected ? selected_color : unselected_color;

  // Draw a rectangle around all the text lines.
  drawFilledRoundedRect(render_info.body_outer_rect,fill_color);
  drawRoundedRect(render_info.body_outer_rect);

  // Draw the text lines.

  int n_lines = render_info.text_objects.size();

  for (int line_index=0; line_index!=n_lines; ++line_index) {
    std::string line_error = lineError(node_index,line_index);

    if (line_error.empty()) {
      glColor3f(1,1,1);
    }
    else {
      glColor3f(1,0,0);
    }

    drawText(render_info.text_objects[line_index]);
  }

  // Draw the input labels

  int n_inputs = node.nInputs();
  int n_outputs = node.nOutputs();

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


void QtDiagramEditor::drawAll()
{
  for (NodeIndex index : diagram().existingNodeIndices()) {
    drawNode(index);
  }

  if (aNodeIsFocused()) {
    ViewportLine cursor_line =
      cursorLine(focused_node_index,text_editor.cursorPosition());
    drawLine(cursor_line.start,cursor_line.end);
  }

  if (!selected_node_connector_index.isNull()) {
    drawLine(
      connectorCircle(selected_node_connector_index).center,
      temp_source_pos
    );
  }

  if (maybe_selection_rectangle) {
    drawRect(*maybe_selection_rectangle);
  }
}


void QtDiagramEditor::paintGL()
{
  if (!diagramPtr()) {
    return;
  }

  begin2DDrawing(width(),height());
  drawAll();
}


void QtDiagramEditor::redraw()
{
  update();
}


bool QtDiagramEditor::event(QEvent *event_ptr)
{
  assert(event_ptr);

  if (event_ptr->type()==QEvent::ToolTip) {
    auto help_event_ptr = static_cast<QHelpEvent*>(event_ptr);
    QHelpEvent &help_event = *help_event_ptr;
    QPoint pos = help_event.globalPos();
    ViewportCoords p = screenToViewportCoords(help_event.x(),help_event.y());
    Optional<string> maybe_tool_tip_text = maybeToolTipTextAt(p);

    if (maybe_tool_tip_text) {
      QToolTip::showText(
        pos,
        QString::fromStdString(*maybe_tool_tip_text),
        this,
        QRect(pos,pos)
      );
    }
  }

  return QGLWidget::event(event_ptr);
}
