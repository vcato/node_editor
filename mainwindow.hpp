#include "treeeditor.hpp"
#include "fileaccessor.hpp"
#include "world.hpp"
#include "worldwrapper.hpp"


class MainWindow {
  public:
    void setWorldPtr(World *);
    void setFileAccessorPtr(FileAccessor *);

  protected:
    void _openProjectPressed();
    void _saveProjectPressed();

  private:
    virtual TreeEditor &treeEditor() = 0;
    virtual Optional<std::string> _askForSavePath() = 0;
    virtual std::string _askForOpenPath() = 0;
      // maybe this should return an
      // optional string to make it more clear that this can fail
      // (i.e. user cancels)
    virtual void _showError(const std::string &message) = 0;

    FileAccessor *_file_accessor_ptr = nullptr;
    std::unique_ptr<WorldWrapper> _world_wrapper_ptr;
};
