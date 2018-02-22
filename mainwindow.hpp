#include "treeeditor.hpp"


class MainWindow {
  public:
    void setWorldPtr(Wrapper *);

  private:
    virtual TreeEditor &treeEditor() = 0;
};
