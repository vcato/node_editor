#include "qtslider.hpp"

#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QSlider>
#include "qtwidget.hpp"
#include "qtlayout.hpp"

using std::cerr;

int main(int argc,char** argv)
{
  QApplication app(argc,argv);
  QWidget window;
  QHBoxLayout &layout = createLayout<QHBoxLayout>(window);
  QtSlider &slider = createWidget<QtSlider>(layout);
  slider.value_changed_function =
    [](int value){ cerr << "Value changed: value=" << value << "\n"; };
  window.show();

  app.exec();
}
