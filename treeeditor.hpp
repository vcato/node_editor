#ifndef TREEEDITOR_HPP_
#define TREEEDITOR_HPP_

#include "wrapper.hpp"
#include "diagrameditorwindow.hpp"


struct TreeEditor {
  public:
    void setWorldPtr(Wrapper *arg) { world_ptr = arg; }
    Wrapper *worldPtr() const { return world_ptr; }
    int nDiagramEditorWindows() const;
    void setWorldState(const WrapperState &);

  protected:
    Wrapper *world_ptr = 0;

    Wrapper &world();
    std::vector<std::string> operationNames(const TreePath &path);

    void setEnumerationIndex(const TreePath &path,int index);
    void executeOperation(const TreePath &,int operation_index);
    void stringItemValueChanged(const TreePath &path,const std::string &value);
    void numberItemValueChanged(const TreePath &path,int value);
    void addTreeItem(const TreePath &new_item_path);
    void addChildTreeItems(const TreePath &parent_path);
    void diagramEditorClosed(DiagramEditorWindow &);
    void openDiagramEditor(const TreePath &);
    virtual void removeTreeItem(const TreePath &path) = 0;
    virtual void removeChildItems(const TreePath &path) = 0;

  private:
    struct TreeObserver;
    virtual void removeDiagramEditors(const TreePath &);

  private:
    std::vector<DiagramEditorWindow *> diagram_editor_window_ptrs;
    using NumericValue = NumericWrapper::Value;

    virtual void
      addWrapperItem(const TreePath &new_item_path,const Wrapper &) = 0;
    virtual void changeEnumerationValues(const TreePath &) = 0;
    virtual DiagramEditorWindow& createDiagramEditor() = 0;

  private:
    void diagramChanged(DiagramEditorWindow &window);
    void notifyItemsOfDiagramChange(Diagram &diagram_that_changed);
    void addMainTreeItem(const TreePath &new_item_path);
    void replaceTreeItems(const TreePath &parent_path);
};

#endif /* TREEEDITOR_HPP_ */
