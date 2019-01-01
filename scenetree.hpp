#ifndef SCENETREE_HPP_
#define SCENETREE_HPP_

#include "scene.hpp"
#include "itemfrompath.hpp"




class SceneTree {
  public:
    struct ItemData {
      using Children = std::vector<ItemData>;

      std::string label;
      Children children;
    };

    void notifySceneChanged();
    void notifyBodyAdded(const Scene::Body &body);
    void notifyRemovingBody(const Scene::Body &body);
    void setScenePtr(Scene *);

  public:

    template <typename Item>
    static void
      insertBodyIn(
        Item &parent_item,
        int index,
        const SceneTree::ItemData &item
      )
    {
      Item &item1 = insertChildItem(parent_item,index);
      setText(item1,item.label);
      addBodiesTo(item1,item.children);
    }

    template <typename Item>
    static void removeBodyFrom(Item &parent_item,int index)
    {
      removeChildItem(parent_item,index);
    }

    template <typename Item>
    static void
      addBodiesTo(
        Item &parent_item,
        const SceneTree::ItemData::Children &item_children
      )
    {
      for (auto &item : item_children) {
        int index = parent_item.childCount();
        insertBodyIn(parent_item,index,item);
      }
    }

  private:
    Scene *scene_ptr = 0;

    Scene &scene();
    virtual void setItems(const ItemData &root) = 0;
    virtual void insertItem(const std::vector<int> &path,const ItemData &) = 0;
    virtual void removeItem(const std::vector<int> &path) = 0;
    void updateItems();
    std::vector<int> bodyPath(const Scene::Body &body);
};

#endif /* SCENETREE_HPP_ */
