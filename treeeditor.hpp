#ifndef TREEEDITOR_HPP_
#define TREEEDITOR_HPP_

#include "wrapper.hpp"


struct TreeEditor {
  public:
    void setWorldPtr(Wrapper *arg) { world_ptr = arg; }

  protected:
    struct OperationHandler;
    Wrapper *world_ptr = 0;

    Wrapper &world();
    std::vector<std::string> operationNames(const TreePath &path);

    void setEnumerationIndex(const TreePath &path,int index);
    void executeOperation(const TreePath &,int operation_index);
    void stringItemValueChanged(const TreePath &path,const std::string &value);
    void addTreeItem(const TreePath &new_item_path);
    void addChildTreeItems(const TreePath &parent_path);
    virtual void removeTreeItem(const TreePath &path) = 0;

  private:
    virtual void addMainTreeItem(const TreePath &new_item_path) = 0;
    virtual void replaceTreeItems(const TreePath &parent_path) = 0;
    virtual void changeEnumerationValues(const TreePath &) = 0;
};

#endif /* TREEEDITOR_HPP_ */
