#ifndef QTTREEEDITOR_HPP_
#define QTTREEEDITOR_HPP_

#include <QLabel>
#include <QString>
#include <QTreeWidgetItem>
#include "qtwidget.hpp"
#include "qtdiagrameditor.hpp"
#include "wrapper.hpp"



class QtTreeEditor : public QTreeWidget {
  Q_OBJECT

  public:
    QtTreeEditor();

    void setWorldPtr(Wrapper *arg) { world_ptr = arg; }
    void setDiagramEditorPtr(QtDiagramEditor *arg) { diagram_editor_ptr = arg; }
    void selectItem(const TreePath &path);

  private slots:
    void itemSelectionChangedSlot();
    void prepareMenuSlot(const QPoint &pos);

  private:
    struct CreateChildItemVisitor;
    struct OperationHandler;

    Wrapper *world_ptr = 0;
    QtDiagramEditor *diagram_editor_ptr = 0;

    Wrapper &world();
    QtDiagramEditor &diagramEditor();
    QtTreeEditor &treeEditor() { return *this; }
    Diagram *maybeSelectedDiagram();
    QTreeWidgetItem &itemFromPath(const TreePath &path) const;
    TreePath itemPath(QTreeWidgetItem &item);
    void buildPath(TreePath &path,QTreeWidgetItem &item);
    void prepareMenu(const QPoint &pos);

    template <typename T>
    T &createItemWidget(QTreeWidgetItem &item,const std::string &label)
    {
      QWidget *wrapper_widget_ptr = new QWidget();
      QHBoxLayout &layout = createLayout<QHBoxLayout>(*wrapper_widget_ptr);
      QLabel &label_widget = createWidget<QLabel>(layout);
      label_widget.setText(QString::fromStdString(label));
      T& widget = createWidget<T>(layout);
      setItemWidget(&item,/*column*/0,wrapper_widget_ptr);
      return widget;
    }

    static QTreeWidgetItem&
      createChildItem(QTreeWidgetItem &parent_item,const std::string &label);

    static QTreeWidgetItem& createChildItem(QTreeWidgetItem &parent_item);

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
      createSpinBoxItem(QTreeWidgetItem &parent_item,const std::string &label);

    QTreeWidgetItem* findSelectedItem();

    void addTreeItem(const TreePath &new_item_path);
    void addTreeItems(const TreePath &parent_path);

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

    void removeChildItems(const TreePath &path);

    void replaceTreeItems(const TreePath &parent_path);
    void changeEnumerationValues(const TreePath &);
};

#endif /* QTTREEEDITOR_HPP_ */
