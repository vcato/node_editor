#include <QObject>


class QtComboBoxTreeWidgetItem;

class QtComboBoxTreeWidgetItemSignalMap : public QObject {
  Q_OBJECT

  public:
    QtComboBoxTreeWidgetItemSignalMap(QtComboBoxTreeWidgetItem &item)
    : item_ptr(&item)
    {
    }

  public slots:
    void currentIndexChangedSlot(int);

  signals:
    void currentIndexChanged(QtComboBoxTreeWidgetItem *,int);

  private:
    QtComboBoxTreeWidgetItem *item_ptr;
};
