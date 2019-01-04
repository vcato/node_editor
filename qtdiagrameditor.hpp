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
    ViewportCoords screenToViewportCoords(int x,int y) const;

    bool
      contains(
        const DiagramTextObject &text_object,
        const ViewportCoords &p
      );

    void mousePressEvent(QMouseEvent *event_ptr) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent * event_ptr) override;
    void drawAll();
    void drawClosedLine(const std::vector<ViewportCoords> &vertices);
    void drawPolygon(const std::vector<ViewportCoords> &vertices);
    void drawPolygon(const std::vector<ViewportCoords> &vertices,const Color &);
    static std::vector<ViewportCoords> verticesOf(const ViewportRect &rect);
    static std::vector<ViewportCoords>
      roundedVerticesOf(const ViewportRect &rect,float offset);
    std::vector<ViewportCoords> verticesOf(const Circle &circle);
    void drawRect(const ViewportRect &arg);
    void drawRoundedRect(const ViewportRect &arg);
    void drawCircle(const Circle &circle);
    void drawFilledRect(const ViewportRect &rect);
    void drawFilledRoundedRect(const ViewportRect &rect,const Color &);
    void drawFilledCircle(const Circle &circle);
    ViewportRect rectAroundText(const ViewportTextObject &text_object) const;

    void
      drawAlignedText(
        const std::string &text,
        const ViewportCoords &position,
        float horizontal_alignment,
        float vertical_alignment
      );

    void drawText(const ViewportTextObject &text_object);
    void
      drawBoxedText2(
        const ViewportTextObject &text_object,
        bool is_selected,
        const ViewportRect &
      );
    void drawBoxedText(const DiagramTextObject &text_object,bool is_selected);
    int textHeight() const;
    int textWidth(const std::string &s) const;
    static constexpr float node_input_radius = 5;

    Circle connectorCircle(NodeConnectorIndex) const;
    void drawNode(NodeIndex);

    void paintGL() override;
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
