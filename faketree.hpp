#include <string>
#include <vector>


struct FakeTree {
  struct Item {
    std::string label;
    std::vector<Item> children;

    const Item* child(int i) const { return &children[i]; }
  };

  Item root;
};
