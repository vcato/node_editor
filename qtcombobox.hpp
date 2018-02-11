#ifndef QTCOMBOBOX_HPP_
#define QTCOMBOBOX_HPP_

#include <functional>
#include <QComboBox>


class QtComboBox : public QComboBox {
  Q_OBJECT

  public:
    QtComboBox();
    std::function<void(int)> current_index_changed_function;
    bool ignore_signals;

  public slots:
    void currentIndexChangedSlot(int);
};

#endif /* QTCOMBOBOX_HPP_ */
