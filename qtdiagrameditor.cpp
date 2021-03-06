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
#include "viewportgeo.hpp"
#include "viewportdraw.hpp"


auto QtDiagramEditor::screenToViewportCoords(int x,int y) const
  -> ViewportPoint
{
  return screenToViewportCoords2(x,y,width(),height());
}



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


void QtDiagramEditor::mousePressEvent(QMouseEvent *event_ptr)
{
  assert(event_ptr);
  QMouseEvent &event = *event_ptr;
  ViewportPoint p = screenToViewportCoords(event.x(),event.y());

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
  ViewportPoint mouse_release_position =
    screenToViewportCoords(event_ptr->x(),event_ptr->y());

  mouseReleasedAt(mouse_release_position);
}


void QtDiagramEditor::mouseMoveEvent(QMouseEvent * event_ptr)
{
  ViewportPoint mouse_position =
    screenToViewportCoords(event_ptr->x(),event_ptr->y());

  mouseMovedTo(mouse_position);
}


void QtDiagramEditor::drawPolygon(const std::vector<ViewportPoint> &vertices)
{
  ::drawPolygon(vertices,Color{0.5,0.5,0});
}


void QtDiagramEditor::drawFilledRect(const ViewportRect &rect)
{
  drawPolygon(verticesOfRect(rect));
}


void QtDiagramEditor::drawFilledCircle(const ViewportCircle &circle)
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

  ViewportPoint p = text_object.position + ViewportVector{text_width,-descent};
  ViewportLine cursor_line{ p, p + ViewportVector{0,cursor_height} };
  return cursor_line;
}


ViewportRect QtDiagramEditor::rectAroundText(const std::string &text_arg) const
{
  std::string text = text_arg;

  if (text == "") {
    // We want to avoid the box collapsing down to nothing.
    text = " ";
  }

  QFontMetrics fm = fontMetrics();
  auto rect = fm.boundingRect(qString(text));

  auto tl = rect.topLeft();
  float bx0 = tl.x();
  float ex0 = tl.x() + rect.width();
  float by0 = -tl.y() - rect.height();
  float ey0 = -tl.y();
  ViewportRect r0 = {{bx0,by0},{ex0,ey0}};

  return r0;
}


void QtDiagramEditor::drawText(const ViewportTextObject &text_object)
{
  ViewportPoint position = text_object.position;
  renderText(position.x,position.y,0,qString(text_object.text));
}


void QtDiagramEditor::drawRect(const ViewportRect &rect)
{
  ::drawRect(rect);
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
    ViewportCircle c = render_info.input_connector_circles[i];
    drawCircle(c);

    if (node_index==selected_node_connector_index.node_index &&
        i==selected_node_connector_index.input_index) {
      drawFilledCircle(c);
    }

    const Node::Input &input = node.inputs[i];

    if (input.source_node_index>=0) {
      const Node& source_node = this->node(input.source_node_index);
      int source_output_index = input.source_output_index;
      ViewportCircle source_circle =
        nodeOutputCircle(source_node,source_output_index);
      drawLine({source_circle.center, c.center});
    }
  }

  // Draw the output connectors

  for (int i=0; i!=n_outputs; ++i) {
    ViewportCircle c = render_info.output_connector_circles[i];
    drawCircle(c);
    if (node_index==selected_node_connector_index.node_index &&
        i==selected_node_connector_index.output_index) {
      drawFilledCircle(c);
    }
  }
}


void QtDiagramEditor::drawLine(const ViewportLine &cursor_line)
{
  ::drawLine(cursor_line.start, cursor_line.end);
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
    ViewportPoint p = screenToViewportCoords(help_event.x(),help_event.y());
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
