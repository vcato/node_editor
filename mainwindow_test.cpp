#include "mainwindow.hpp"

#include <string>


using std::string;


namespace {
struct FakeTreeEditor : TreeEditor {
  void userSelectsContextMenuItem(const string &)
  {
  }
};
}


namespace {
struct FakeMainWindow : MainWindow {
  FakeTreeEditor &treeEditor() override { return tree_editor; }

  FakeTreeEditor tree_editor;
};
}


int main()
{
#if 0
  FakeMainWindow main_window;
  main_window.tree_editor.userSelectsContextMenuItem("Add Scene");
#endif

  // User select Add Scene in the tree editor.
  // Use selects Add Body on the scene.
  // Assert the scene window shows a body in the tree.
}
