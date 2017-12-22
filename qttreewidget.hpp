#ifndef QTTREEWIDGET_HPP_
#define QTTREEWIDGET_HPP_

#include <QLabel>
#include <QString>
#include <QSpinBox>
#include <QTreeWidgetItem>
#include <QComboBox>
#include "qtwidget.hpp"
#include "qtcomboboxtreewidgetitemsignalmap.hpp"


struct QtComboBoxTreeWidgetItem : QTreeWidgetItem {
  QtComboBoxTreeWidgetItem()
  : combo_box_ptr(0),
    signal_map(*this)
  {
  }

  QComboBox &comboBox()
  {
    assert(combo_box_ptr);
    return *combo_box_ptr;
  }

  QComboBox *combo_box_ptr;
  QtComboBoxTreeWidgetItemSignalMap signal_map;
};


class QtTreeWidget : public QTreeWidget {
  Q_OBJECT

  public:
  template <typename T>
  T &setItemWidget(QTreeWidgetItem &item,const std::string &label)
  {
    QTreeWidget &tree_widget = *this;
    QWidget *wrapper_widget_ptr = new QWidget();
    QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);
    QLabel &label_widget = createWidget<QLabel>(layout);
    label_widget.setText(QString::fromStdString(label));
    T& widget = createWidget<T>(layout);
    tree_widget.setItemWidget(&item,/*column*/0,wrapper_widget_ptr);
    return widget;
  }

  static QTreeWidgetItem& createItem(QTreeWidgetItem &parent_item)
  {
    QTreeWidgetItem *item_ptr = new QTreeWidgetItem;
    parent_item.addChild(item_ptr);
    QTreeWidgetItem &item = *item_ptr;
    item.setExpanded(true);
    return item;
  }

  QtComboBoxTreeWidgetItem&
    createComboBoxItem(QTreeWidgetItem &parent_item,const std::string &label)
  {
    QtComboBoxTreeWidgetItem *item_ptr = new QtComboBoxTreeWidgetItem;
    parent_item.addChild(item_ptr);
    QtComboBoxTreeWidgetItem &item = *item_ptr;
    item.setExpanded(true);
    QComboBox &combo_box = setItemWidget<QComboBox>(item,label);
    item.combo_box_ptr = &combo_box;
    item.signal_map.connect(
      &combo_box,
      SIGNAL(currentIndexChanged(int)),
      SLOT(currentIndexChangedSlot(int))
    );
    connect(
      &item.signal_map,
      SIGNAL(currentIndexChanged(QtComboBoxTreeWidgetItem*,int)),
      SLOT(comboBoxItemCurrentIndexChangedSlot(QtComboBoxTreeWidgetItem*,int))
    );
    return item;
  }

  void
    createSpinBoxItem(
      QTreeWidgetItem &parent_item,
      const std::string &label
    )
  {
    QtTreeWidget &tree_widget = *this;
    QTreeWidgetItem &item = createItem(parent_item);
    tree_widget.setItemWidget<QSpinBox>(item,label);
  }

  private slots:
    void comboBoxItemCurrentIndexChangedSlot(QtComboBoxTreeWidgetItem *,int);

  signals:
    void comboBoxItemIndexChanged(QtComboBoxTreeWidgetItem*,int);

};

#endif /* QTTREEWIDGET_HPP_ */
