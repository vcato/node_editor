#include <QTreeWidget>
#include "scenetree.hpp"


class QtSceneTree : public QTreeWidget, public SceneTree {
  public:
    QtSceneTree();
    void setItems(const Item &root) override;
};
