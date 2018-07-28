#include "treeeditor.hpp"


class MainWindow {
  public:
    void setWorldPtr(Wrapper *);

  protected:
    void _openProjectPressed();
    void _saveProjectPressed();

  private:
    virtual TreeEditor &treeEditor() = 0;
    virtual std::string _askForSavePath() = 0;
      // maybe this should return an
      // optional string to make it more clear that this can fail
      // (i.e. user cancels)
    virtual std::string _askForOpenPath() = 0;
      // maybe this should return an
      // optional string to make it more clear that this can fail
      // (i.e. user cancels)
    virtual void _showError(const std::string &message) = 0;
};
