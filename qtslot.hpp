#include <QObject>
#include <functional>


class QtSlot : public QObject {
  Q_OBJECT

  public:
    QtSlot(std::function<void()> function_arg);

    void connectSignal(QObject &object,const char *signal)
    {
      connect(&object,signal,SLOT(slot()));
    }

  public slots:
    void slot();

  private:
    std::function<void()> slot_function;
};
