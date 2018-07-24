#ifndef QTTREEEDITOR_HPP_
#define QTTREEEDITOR_HPP_

#include <QLabel>
#include <QString>
#include <QTreeWidgetItem>
#include "qtwidget.hpp"
#include "qtdiagrameditor.hpp"
#include "qtdiagrameditorwindow.hpp"
#include "treeeditor.hpp"



class QtTreeEditor : public QTreeWidget, public TreeEditor {
  Q_OBJECT

  public:
    QtTreeEditor();

    void selectItem(const TreePath &path);

  private slots:
    void itemSelectionChangedSlot();
    void prepareMenuSlot(const QPoint &pos);

  private:
    struct CreateChildItemVisitor;

    QtTreeEditor &treeEditor() { return *this; }
    Diagram *maybeSelectedDiagram();
    QTreeWidgetItem &itemFromPath(const TreePath &path) const;
    TreePath itemPath(QTreeWidgetItem &item);
    void buildPath(TreePath &path,QTreeWidgetItem &item);
    void prepareMenu(const QPoint &pos);
    QtDiagramEditorWindow& createDiagramEditor() override;

    template <typename T>
    T &createItemWidget(QTreeWidgetItem &item,const std::string &label)
    {
      QWidget *wrapper_widget_ptr = new QWidget();
      // NOTE: setting the item widget before adding the contents makes
      // it not have the proper size.
      QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);
      QLabel &label_widget = createWidget<QLabel>(layout);
      label_widget.setText(QString::fromStdString(label));
      T& widget = createWidget<T>(layout);
      setItemWidget(&item,/*column*/0,wrapper_widget_ptr);
      return widget;
    }

    static QTreeWidgetItem&
      createChildItem(QTreeWidgetItem &parent_item,const std::string &label);

    static void setItemText(QTreeWidgetItem &item,const std::string &label);

    QTreeWidgetItem&
      createComboBoxItem(
        QTreeWidgetItem &parent_item,
        const std::string &label,
        const std::vector<std::string> &enumeration_names
      );

    void
      createLineEditItem(
        QTreeWidgetItem &,
        const std::string &label,
        const std::string &value
      );

    void
      createSpinBoxItem(
        QTreeWidgetItem &parent_item,
        const std::string &label,
        int value
      );

    QTreeWidgetItem* findSelectedItem();

    void addWrapperItem(const TreePath &new_item_path,const Wrapper &) override;
    void removeTreeItem(const TreePath &path) override;
    void removeChildItems(const TreePath &path) override;

    void
      handleComboBoxItemIndexChanged(
        QTreeWidgetItem *item_ptr,
        int index
      );

    void
      handleSpinBoxItemValueChanged(
        QTreeWidgetItem *item_ptr,
        int value
      );

    void
      handleLineEditItemValueChanged(
        QTreeWidgetItem *item_ptr,
        const std::string &value
      );

    void changeEnumerationValues(const TreePath &) override;
};

#endif /* QTTREEEDITOR_HPP_ */
