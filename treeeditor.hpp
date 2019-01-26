#ifndef TREEEDITOR_HPP_
#define TREEEDITOR_HPP_

#include "diagrameditorwindow.hpp"
#include "wrapperstate.hpp"
#include "treepath.hpp"

struct Wrapper;


struct TreeEditor {
  public:
    void setWorldPtr(Wrapper *arg);
    Wrapper *worldPtr() const { return world_ptr; }
    int nDiagramEditorWindows() const;
    void setWorldState(const WrapperState &);

  protected:
    struct MenuItem {
      std::string label;
      std::function<void()> callback;
    };

    struct LabelProperties {
      std::string text;
      bool is_editable = false;
    };

    Wrapper &world();
    std::vector<std::string> operationNames(const TreePath &path);

    void setEnumerationIndex(const TreePath &path,int index);
    void executeOperation(const TreePath &,int operation_index);
    void stringItemValueChanged(const TreePath &path,const std::string &value);
    void numberItemValueChanged(const TreePath &path,int value);
    void
      itemLabelChanged(
        const TreePath &path,
        const std::string &new_item_text
      );
    const Optional<TreePath> &maybePathOfItemBeingEdited() const;

    void createTreeItem(const TreePath &new_item_path);
    void addChildTreeItems(const TreePath &parent_path);
    void diagramEditorClosed(DiagramEditorWindow &);
    void openDiagramEditor(const TreePath &);
    std::vector<MenuItem> contextMenuItems(const TreePath &path);

    virtual void removeTreeItem(const TreePath &path) = 0;
    virtual void removeChildItems(const TreePath &path) = 0;
    virtual void
      setItemExpanded(const TreePath &path,bool new_expanded_state) = 0;

  private:
    struct CreateChildItemVisitor;
    struct TreeObserver;
    virtual void removeDiagramEditors(const TreePath &);
    void addWrapperItem(const TreePath &new_item_path,const Wrapper &);
    virtual int itemChildCount(const TreePath &parent_item) const = 0;
    void changeEnumerationValues(const TreePath &);

    virtual void
      setEnumerationValues(
        const TreePath &path,
        const std::vector<std::string> &items
      ) = 0;

    virtual DiagramEditorWindow& createDiagramEditor() = 0;

    // Maybe having a LabelProperties struct is best.
    virtual void
      createVoidItem(
        const TreePath &new_item_path,
        const LabelProperties &label_properties
      ) = 0;

    virtual void
      createNumericItem(
        const TreePath &new_item_path,
        const LabelProperties &,
        const NumericValue value
      ) = 0;

    virtual void
      createEnumerationItem(
        const TreePath &new_item_path,
        const LabelProperties &,
        const std::vector<std::string> &options,
        int value
      ) = 0;

    virtual void
      createStringItem(
        const TreePath &new_item_path,
        const LabelProperties &,
        const std::string &value
      ) = 0;

    void addMainTreeItem(const TreePath &new_item_path);
    void replaceChildTreeItems(const TreePath &parent_path);
    void collapseBranch(const TreePath &path);
    void collapseChildren(const TreePath &path);

    Wrapper *world_ptr = 0;
    std::vector<DiagramEditorWindow *> diagram_editor_window_ptrs;
    Optional<TreePath> maybe_path_of_item_being_edited;
};

#endif /* TREEEDITOR_HPP_ */
