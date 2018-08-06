#ifndef QTDIAGRAMEDITOR_HPP_
#define QTDIAGRAMEDITOR_HPP_

#include <cassert>
#include <QGLWidget>
#include "diagrameditor.hpp"
#include "circle.hpp"


class QKeyEvent;


struct Color {
  float r,g,b;
};


class QtDiagramEditor : public QGLWidget, public DiagramEditor {
   Q_OBJECT

  public:
    QtDiagramEditor();

    void redraw() override { update(); }

  private slots:
    void exportDiagramSlot();
    void importDiagramSlot();

  private:
    void initializeGL() override { }
    QSize sizeHint() const override { return QSize(640,480); }
    void keyPressEvent(QKeyEvent *key_event_ptr) override;
#if 0
    Point2D screenToViewportCoords(int x,int y) const;
#else
    ViewportCoords screenToViewportCoords(int x,int y) const;
#endif
    bool contains(const TextObject &text_object,const Point2D &p);
    void mousePressEvent(QMouseEvent *event_ptr) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent * event_ptr) override;
    void drawAll();
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

    Circle connectorCircle(NodeConnectorIndex) const;
    TextObject
      outputTextObject(const std::string &s,float right_x,float y) const;
    void drawNode(NodeIndex);
    void paintGL() override;

    void showError(const std::string &message) override;
    std::string askForSavePath() override;
    std::string askForOpenPath() override;
};

#endif /* QTDIAGRAMEDITOR_HPP_ */
