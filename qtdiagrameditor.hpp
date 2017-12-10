#ifndef QTDIAGRAMEDITOR_HPP_
#define QTDIAGRAMEDITOR_HPP_

#include <cassert>
#include <GL/glu.h>
#include <QGLWidget>
#include <QKeyEvent>
#include "diagrameditor.hpp"
#include "circle.hpp"


struct NodeRenderInfo {
  Rect header_rect;
  Rect body_outer_rect;
  std::vector<TextObject> text_objects;
  std::vector<Circle> input_connector_circles;
  std::vector<Circle> output_connector_circles;
};



struct Color {
  float r,g,b;
};


class QtDiagramEditor : public QGLWidget, public DiagramEditor {
  public:
    QtDiagramEditor(Diagram &);

  private:
    void initializeGL() override { }
    QSize sizeHint() const override { return QSize(640,480); }
    void keyPressEvent(QKeyEvent *key_event_ptr) override;
    Point2D screenToGLCoords(int x,int y) const;
    bool contains(const TextObject &text_object,const Point2D &p);
    NodeIndex indexOfNodeContaining(const Point2D &p);
    bool nodeInputContains(int node_index,int input_index,const Point2D &p);
    bool nodeOutputContains(int node_index,int output_index,const Point2D &p);
    NodeConnectorIndex indexOfNodeConnectorContaining(const Point2D &p);
    void mousePressedAt(Point2D p);
    void mousePressEvent(QMouseEvent *event_ptr) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent * event_ptr) override;
    void drawLine(Point2D line_start,Point2D line_end);
    void setupProjection(float viewport_width,float viewport_height);
    void drawClosedLine(const std::vector<Point2D> &vertices);
    void drawPolygon(const std::vector<Point2D> &vertices);
    void drawPolygon(const std::vector<Point2D> &vertices,const Color &);
    static std::vector<Point2D> verticesOf(const Rect &rect);
    static std::vector<Point2D>
      roundedVerticesOf(const Rect &rect,float offset);
    std::vector<Point2D> verticesOf(const Circle &circle);
    void drawRect(const Rect &arg);
    void drawRoundedRect(const Rect &arg);
    void drawCircle(const Circle &circle);
    void drawFilledRect(const Rect &rect);
    void drawFilledRoundedRect(const Rect &rect,const Color &);
    void drawFilledCircle(const Circle &circle);
    Rect rectAroundText(const TextObject &text_object) const;
    Rect nodeRect(const TextObject &text_object) const;
    Rect nodeHeaderRect(const TextObject &text_object) const;

    Point2D
      alignmentPoint(
        const Rect &rect,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    TextObject
      alignedTextObject(
        const std::string &text,
        const Point2D &position,
        float horizontal_alignment,
        float vertical_alignment
      ) const;

    void
      drawAlignedText(
        const std::string &text,
        const Point2D &position,
        float horizontal_alignment,
        float vertical_alignment
      );

    void drawText(const TextObject &text_object);
    void
      drawBoxedText2(
        const TextObject &text_object,
        bool is_selected,
        const Rect &
      );
    void drawBoxedText(const TextObject &text_object,bool is_selected);
    int textHeight() const;
    int textWidth(const std::string &s) const;
    void drawCursor(const TextObject &text_object);
    void drawCursor(const TextObject &text_object,int column_index);
    static constexpr float node_input_radius = 5;
    static constexpr float connector_radius = 5;

    Circle nodeInputCircle(const Node &,int input_index);
    Circle nodeOutputCircle(const Node &node,int output_index);
    Circle connectorCircle(NodeConnectorIndex) const;
    Point2D nodeOutputPosition(int node_index);
    void drawNodeInput(int node_index,int input_index);
    void drawNodeInputs(int node_index);
    TextObject
      inputTextObject(const std::string &s,float left_x,float y) const;
    TextObject
      outputTextObject(const std::string &s,float right_x,float y) const;
    Rect nodeBodyRect(const Node &,const Rect &header_rect) const;
    NodeRenderInfo nodeRenderInfo(const Node &node) const;
    void drawNode(NodeIndex);
    void paintGL() override;
    void redraw() override { update(); }
};

#endif /* QTDIAGRAMEDITOR_HPP_ */
