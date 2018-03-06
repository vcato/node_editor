#include <QLineEdit>
#include <functional>


class QtLineEdit : public QLineEdit {
  Q_OBJECT

  public:
    QtLineEdit();

    std::function<void(std::string)> text_changed_function;

  private slots:
    void textChangedSlot(const QString &);
};
