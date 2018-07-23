#include <string>
#include <vector>

struct FakeTree {
  struct Item {
    std::string label;
    std::vector<Item> children;
  };

  std::vector<Item> children;
};
