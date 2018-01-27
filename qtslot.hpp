#include <QObject>
#include <functional>


class QtSlot : public QObject {
  Q_OBJECT

  public:
    QtSlot(std::function<void()> function_arg);

  public slots:
    void slot();

  private:
    std::function<void()> slot_function;
};
