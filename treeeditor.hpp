#ifndef TREEEDITOR_HPP_
#define TREEEDITOR_HPP_

#include "diagrameditorwindow.hpp"
#include "wrapperstate.hpp"
#include "treepath.hpp"
#include "treewidget.hpp"
#include "treeupdating.hpp"

struct Wrapper;


struct TreeEditor {
  public:
    void setWorldPtr(Wrapper *arg);
    Wrapper *worldPtr() const { return world_ptr; }
    int nDiagramEditorWindows() const;

    void setWorldState(const WrapperState &);
      // This allows us to tell the tree editor that we want to change the
      // state of the world wrapper to the given state.  It applies the
      // new state to the wrapper and updates the tree.

    void itemValueChanged(const TreePath &);

  protected:
    struct MenuItem {
      std::string label;
      std::function<void()> callback;
    };

    using LabelProperties = TreeWidget::LabelProperties;

    Wrapper &world();
    std::vector<std::string> operationNames(const TreePath &path);

    void setEnumerationIndex(const TreePath &path,int index);
    void executeOperation(const TreePath &,int operation_index);
    void stringItemValueChanged(const TreePath &path,const std::string &value);
    void numberItemValueChanged(const TreePath &path,int value);

    void diagramEditorClosed(DiagramEditorWindow &);
    void openDiagramEditor(const TreePath &);
    std::vector<MenuItem> contextMenuItems(const TreePath &path);

    virtual void removeChildItems(const TreePath &path) = 0;

    virtual void
      setItemExpanded(const TreePath &path,bool new_expanded_state) = 0;

  private:
    virtual void removeDiagramEditors(const TreePath &);
    virtual int itemChildCount(const TreePath &parent_item) const = 0;
    virtual DiagramEditorWindow& createDiagramEditor() = 0;

    virtual TreeWidget &tree() = 0;

    void replaceChildTreeItems(const TreePath &parent_path);
    void collapseBranch(const TreePath &path);
    void collapseChildren(const TreePath &path);

    TreeUpdatingObserver treeObserver(TreeEditor &);

    Wrapper *world_ptr = 0;
    std::vector<DiagramEditorWindow *> diagram_editor_window_ptrs;
};

#endif /* TREEEDITOR_HPP_ */
