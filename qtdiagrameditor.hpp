#ifndef QTDIAGRAMEDITOR_HPP_
#define QTDIAGRAMEDITOR_HPP_

#include <cassert>
#include <QGLWidget>
#include "diagrameditor.hpp"
#include "circle.hpp"
#include "viewportline.hpp"


class QKeyEvent;


struct Color {
  float r,g,b;
};


class QtDiagramEditor : public QGLWidget, public DiagramEditor {
   Q_OBJECT

  public:
    QtDiagramEditor();

    void redraw() override;

  private slots:
    void exportDiagramSlot();
    void importDiagramSlot();

  private:
    void initializeGL() override { }
    QSize sizeHint() const override { return QSize(640,480); }
    void keyPressEvent(QKeyEvent *key_event_ptr) override;
    ViewportPoint screenToViewportCoords(int x,int y) const;
    void mousePressEvent(QMouseEvent *event_ptr) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent * event_ptr) override;
    void drawAll();
    void drawClosedLine(const std::vector<ViewportPoint> &vertices);
    void drawPolygon(const std::vector<ViewportPoint> &vertices);
    void drawPolygon(const std::vector<ViewportPoint> &vertices,const Color &);
    static std::vector<ViewportPoint> verticesOf(const ViewportRect &rect);
    static std::vector<ViewportPoint>
      roundedVerticesOf(const ViewportRect &rect,float offset);
    std::vector<ViewportPoint> verticesOf(const Circle &circle);
    void drawRect(const ViewportRect &arg);
    void drawRoundedRect(const ViewportRect &arg);
    void drawCircle(const Circle &circle);
    void drawFilledRect(const ViewportRect &rect);
    void drawFilledRoundedRect(const ViewportRect &rect,const Color &);
    void drawFilledCircle(const Circle &circle);
    ViewportRect rectAroundText(const std::string &text) const;
    ViewportRect
      rectAroundTextObject(const ViewportTextObject &text_object) const;

    void drawText(const ViewportTextObject &text_object);
    void
      drawBoxedText2(
        const ViewportTextObject &text_object,
        bool is_selected,
        const ViewportRect &
      );
    int textHeight() const;
    int textWidth(const std::string &s) const;
    static constexpr float node_input_radius = 5;

    Circle connectorCircle(NodeConnectorIndex) const;
    void drawNode(NodeIndex);

    void paintGL() override;
    bool event(QEvent *) override;

    std::string askForSavePath() override;
    std::string askForOpenPath() override;
    void showError(const std::string &message) override;

    ViewportLine
      textObjectCursorLine(
        const ViewportTextObject &text_object,
        int column_index
      ) const override;
};

#endif /* QTDIAGRAMEDITOR_HPP_ */
