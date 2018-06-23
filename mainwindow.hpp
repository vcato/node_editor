#include "treeeditor.hpp"


class MainWindow {
  public:
    void setWorldPtr(Wrapper *);

  protected:
    void _saveProjectPressed();

  private:
    virtual TreeEditor &treeEditor() = 0;
    virtual std::string _askForSavePath() = 0;
};
