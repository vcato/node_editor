#include <cassert>
#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QGLWidget>
#include <QMouseEvent>
#include <GL/glu.h>

using std::cerr;
using std::string;
using std::vector;


namespace {
struct Point2D {
  float x,y;
};
}


static Point2D operator-(const Point2D &a,const Point2D &b)
{
  float x = a.x - b.x;
  float y = a.y - b.y;
  return Point2D{x,y};
}


static Point2D operator+(const Point2D &a,const Point2D &b)
{
  float x = a.x + b.x;
  float y = a.y + b.y;
  return Point2D{x,y};
}


namespace {
struct Rect {
  Point2D start, end;

  bool contains(const Point2D &p)
  {
    return
      p.x >= start.x && p.x <= end.x &&
      p.y >= start.y && p.y <= end.y;
  }
};
}


static float distanceBetween(const Point2D &a,const Point2D &b)
{
  float dx = a.x - b.x;
  float dy = a.y - b.y;

  return sqrtf(dx*dx + dy*dy);
}


namespace {
struct Circle {
  Point2D center;
  float radius;

  bool contains(const Point2D &p)
  {
    return distanceBetween(center,p)<=radius;
  }
};
}


namespace {
struct TextObject {
  string text;
  Point2D position;
};
}


static QString qString(const string &arg)
{
  return QString::fromUtf8(arg.c_str());
}


static Rect
   rectAroundText(
     const TextObject &text_object,
     QGLWidget &widget
  )
{
  string text = text_object.text;
  if (text == "") {
    // We want to avoid the box collapsing down to nothing.
    text = " ";
  }
  auto position = text_object.position;

  QFontMetrics fm = widget.fontMetrics();
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


namespace {
struct NodeInputIndex {
  int node_index;
  int input_index;

  bool operator==(const NodeInputIndex &arg) const
  {
    return node_index==arg.node_index && input_index==arg.input_index;
  }

  bool operator!=(const NodeInputIndex &arg) const
  {
    return !operator==(arg);
  }

  static NodeInputIndex null()
  {
    return NodeInputIndex{-1,0};
  }

  void clear()
  {
    *this = null();
  }

  bool isNull() const
  {
    return *this==null();
  }
};
}


namespace {
struct Node {
  struct Input {
    int source_node_index;

    Input()
    : source_node_index(-1)
    {
    }
  };

  Node()
  : inputs(2)
  {
  }

  TextObject text_object;
  vector<Input> inputs;
};
}


namespace {
class GLWidget : public QGLWidget {
  public:
    GLWidget()
    {
      {
        QFont font;
        font.setPointSize(20);
        setFont(font);
      }
      setFocusPolicy(Qt::StrongFocus);
    }

  private:
    void initializeGL() override {
    }

    QSize sizeHint() const override
    {
      return QSize(640,480);
    }

    void keyPressEvent(QKeyEvent *key_event_ptr) override
    {
      assert(key_event_ptr);
      if (key_event_ptr->key()==Qt::Key_Backspace) {
        if (selected_node_index>=0) {
          nodes.erase(nodes.begin()+selected_node_index);
          selected_node_index = -1;
          update();
          return;
        }

        if (!current_text.text.empty()) {
          current_text.text.erase(current_text.text.end()-1);
        }
      }
      else {
        current_text.text += key_event_ptr->text().toStdString();
      }
      update();
    }

    Point2D screenToGLCoords(int x,int y) const
    {
      return Point2D{static_cast<float>(x),height()-static_cast<float>(y)};
    }

    bool contains(const TextObject &text_object,const Point2D &p)
    {
      return nodeRect(text_object).contains(p);
    }

    int indexOfNodeContaining(const Point2D &p)
    {
      int n_text_objects = nodes.size();

      for (int i=0; i!=n_text_objects; ++i) {
        if (contains(nodes[i].text_object,p)) {
          return i;
        }
      }

      return -1;
    }

    void selectNode(int index)
    {
      selected_node_index = index;
    }

    bool nodeInputContains(int node_index,int input_index,const Point2D &p)
    {
      return defaultNodeInputCircle(node_index,input_index).contains(p);
    }

    NodeInputIndex indexOfNodeInputContaining(const Point2D &p)
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

    void mousePressEvent(QMouseEvent *event_ptr) override
    {
      Point2D p = screenToGLCoords(event_ptr->x(),event_ptr->y());

      if (!current_text.text.empty()) {
        Node node;
        node.text_object = current_text;
        nodes.push_back(node);
      }

      assert(event_ptr);
      current_text.text = "";
      current_text.position = p;

      {
        NodeInputIndex i = indexOfNodeInputContaining(p);

        if (i!=NodeInputIndex::null()) {
          selected_node_input_index = i;
          selected_node_index = -1;
          temp_source_pos = screenToGLCoords(event_ptr->x(),event_ptr->y());
          update();
          return;
        }
      }

      {
        int i = indexOfNodeContaining(p);

        if (i>=0) {
          selectNode(i);
          update();
          return;
        }
      }

      selected_node_index = -1;
      selected_node_input_index = NodeInputIndex::null();

      update();
    }

    void mouseReleaseEvent(QMouseEvent *) override
    {
      if (!selected_node_input_index.isNull()) {
        int source_node_index = indexOfNodeContaining(temp_source_pos);
        nodes[selected_node_input_index.node_index].inputs[
          selected_node_input_index.input_index].source_node_index =
            source_node_index;
        selected_node_input_index.clear();
        update();
      }
    }

    void mouseMoveEvent(QMouseEvent * event_ptr) override
    {
      if (!selected_node_input_index.isNull()) {
        temp_source_pos = screenToGLCoords(event_ptr->x(),event_ptr->y());
        update();
        return;
      }
    }

    void drawLine(Point2D line_start,Point2D line_end)
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

    void setupProjection(float viewport_width,float viewport_height)
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

    void drawClosedLine(const vector<Point2D> &vertices)
    {
      int n = vertices.size();

      for (int i=0; i!=n; ++i) {
        drawLine(vertices[i],vertices[(i+1)%n]);
      }
    }

    void drawPolygon(const vector<Point2D> &vertices)
    {
      int n_vertices = vertices.size();
      int vertex_size = 2;
      int color_size = 3;
      vector<float> vertex_data;
      vertex_data.resize(n_vertices*vertex_size);
      vector<float> color_data;
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
        vector<unsigned int> index_data(n_vertices);
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

    void drawRect(const Rect &arg)
    {
      drawClosedLine(verticesOf(arg));
    }

    static vector<Point2D> verticesOf(const Rect &rect)
    {
      vector<Point2D> vertices;

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

    void drawFilledRect(const Rect &rect)
    {
      drawPolygon(verticesOf(rect));
    }

    void drawFilledCircle(const Circle &circle)
    {
      drawPolygon(verticesOf(circle));
    }

    Rect withMargin(const Rect &rect,float margin)
    {
      auto offset = Point2D{margin,margin};
      return Rect{rect.start-offset,rect.end+offset};
    }

    Rect nodeRect(const TextObject &text_object)
    {
      return withMargin(rectAroundText(text_object,*this),5);
    }

    void drawBoxedText(const TextObject &text_object,bool is_selected)
    {
      Rect rect = nodeRect(text_object);

      if (is_selected) {
        drawFilledRect(rect);
      }

      drawRect(rect);

      Point2D position = text_object.position;

      renderText(position.x,position.y,0,qString(text_object.text));
    }

    int textHeight() const
    {
      return fontMetrics().height();
    }

    int textWidth(const string &s) const
    {
      return fontMetrics().width(qString(s));
    }

    void drawCursor()
    {
      float cursor_height = textHeight();
      float text_width = textWidth(current_text.text);
      float descent = fontMetrics().descent();
      Point2D p =
        current_text.position + Point2D{text_width,-descent};
      drawLine(p,p+Point2D{0,cursor_height});
    }

    vector<Point2D> verticesOf(const Circle &circle)
    {
      Point2D center = circle.center;
      float radius = circle.radius;
      vector<Point2D> vertices;

      for (int i=0; i!=10; ++i) {
        float fraction = i/10.0;
        float angle = 2*M_PI * fraction;
        float x = center.x + cos(angle)*radius;
        float y = center.y + sin(angle)*radius;
        vertices.push_back(Point2D{x,y});
      }

      return vertices;
    }

    void drawCircle(const Circle &circle)
    {
      drawClosedLine(verticesOf(circle));
    }

    static constexpr float node_input_radius = 5;

    Point2D
      defaultNodeInputPosition(
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

    Circle defaultNodeInputCircle(int node_index,int input_index)
    {
      Circle circle;
      circle.center = defaultNodeInputPosition(node_index,input_index);
      circle.radius = node_input_radius;

      return circle;
    }

    Point2D nodeOutputPosition(int node_index)
    {
      Rect rect = nodeRect(nodes[node_index].text_object);
      float x = rect.end.x;
      float y = (rect.start.y + rect.end.y)/2;
      return Point2D{x,y};
    }

    void drawNodeInput(int node_index,int input_index)
    {
      NodeInputIndex node_input_index;
      node_input_index.node_index = node_index;
      node_input_index.input_index = input_index;

      int source_node_index =
        nodes[node_index].inputs[input_index].source_node_index;

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

    void drawNodeInputs(int node_index)
    {
      int n_inputs = nodes[node_index].inputs.size();

      for (int i=0; i!=n_inputs; ++i) {
        drawNodeInput(node_index,i);
      }
    }

    void paintGL() override
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
        drawBoxedText(current_text,/*is_selected*/false);
        drawCursor();
      }

      int n_text_objects = nodes.size();

      for (int index=0; index!=n_text_objects; ++index) {
        auto& text_object = nodes[index].text_object;
        bool is_selected = (selected_node_index==index);
        drawBoxedText(text_object,is_selected);
        drawNodeInputs(index);
      }
    }

    TextObject current_text;
    int selected_node_index = -1;
    NodeInputIndex selected_node_input_index = NodeInputIndex::null();
    Point2D temp_source_pos;

    vector<Node> nodes;
};
}


int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  QMainWindow main_window;
  GLWidget gl_widget;
  main_window.setCentralWidget(&gl_widget);
  main_window.show();
  return app.exec();
}
