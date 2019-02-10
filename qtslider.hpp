#include <QSlider>
#include <functional>


struct QtSlider : QSlider {
  Q_OBJECT

  public:
    QtSlider();

    std::function<void(int)> value_changed_function;

  private slots:
    void valueChangedSlot(int);
};
