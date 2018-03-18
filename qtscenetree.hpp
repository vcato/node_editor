#include <QTreeWidget>
#include "scenetree.hpp"


class QtSceneTree : public QTreeWidget, public SceneTree {
  public:
    QtSceneTree();

  private:
    void setItems(const ItemData &root) override;
    void insertItem(const std::vector<int> &path,const ItemData &) override;
    void removeItem(const std::vector<int> &path) override;
};
