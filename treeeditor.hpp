#ifndef TREEEDITOR_HPP_
#define TREEEDITOR_HPP_

#include "diagrameditorwindow.hpp"
#include "wrapperstate.hpp"
#include "treepath.hpp"

struct Wrapper;

struct TreeEditor {
  public:
    void setWorldPtr(Wrapper *arg) { world_ptr = arg; }
    Wrapper *worldPtr() const { return world_ptr; }
    int nDiagramEditorWindows() const;
    void setWorldState(const WrapperState &);

  protected:
    struct MenuItem {
      std::string label;
      std::function<void()> callback;
    };

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
    std::vector<MenuItem> contextMenuItems(const TreePath &path);

    virtual void removeTreeItem(const TreePath &path) = 0;
    virtual void removeChildItems(const TreePath &path) = 0;

  private:
    struct CreateChildItemVisitor;

    struct TreeObserver;
    virtual void removeDiagramEditors(const TreePath &);

  private:
    std::vector<DiagramEditorWindow *> diagram_editor_window_ptrs;

    void addWrapperItem(const TreePath &new_item_path,const Wrapper &);
    virtual int itemChildCount(const TreePath &parent_item) const = 0;
    void changeEnumerationValues(const TreePath &);

    virtual void
      setEnumerationValues(
        const TreePath &path,
        const std::vector<std::string> &items
      ) = 0;

    virtual DiagramEditorWindow& createDiagramEditor() = 0;

    virtual void
      createVoidItem(
        const TreePath &parent_path,
        const std::string &label
      ) = 0;

    virtual void
      createNumericItem(
        const TreePath &parent_path,
        const std::string &label,
        const NumericValue value
      ) = 0;

    virtual void
      createEnumerationItem(
        const TreePath &parent_path,
        const std::string &label,
        const std::vector<std::string> &options
      ) = 0;

    virtual void
      createStringItem(
        const TreePath &parent_path,
        const std::string &label,
        const std::string &value
      ) = 0;

  private:
    void diagramChanged(DiagramEditorWindow &window);
    void notifyItemsOfDiagramChange(Diagram &diagram_that_changed);
    void addMainTreeItem(const TreePath &new_item_path);
    void replaceTreeItems(const TreePath &parent_path);
};

#endif /* TREEEDITOR_HPP_ */
