#include "mainwindow.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include "wrapperstate.hpp"
#include "optional.hpp"
#include "wrapper.hpp"

using std::cerr;
using std::string;
using std::unique_ptr;
using std::istream;


void MainWindow::setWorldPtr(Wrapper *world_ptr_arg)
{
  treeEditor().setWorldPtr(world_ptr_arg);
}


void MainWindow::_openProjectPressed()
{
  string path = _askForOpenPath();

  if (path=="") {
    return;
  }

  assert(_file_accessor_ptr);
  unique_ptr<istream> maybe_stream =
    _file_accessor_ptr->maybeOpenForRead(path);

  if (!maybe_stream) {
    _showError("Unable to open "+path);
    return;
  }

  istream &stream = *maybe_stream;

  ScanStateResult scan_result = scanStateFrom(stream);

  if (scan_result.isError()) {
    _showError("Unable to read "+path+"\n"+scan_result.asError().message);
    return;
  }

  treeEditor().setWorldState(scan_result.state());
}


void MainWindow::_saveProjectPressed()
{
  Optional<string> maybe_path = _askForSavePath();

  if (!maybe_path) {
    return;
  }

  const string &path = *maybe_path;

  std::ofstream stream(path);

  if (!stream) {
    cerr << "Couldn't open " << path << "\n";
    return;
  }

  Wrapper *world_ptr = treeEditor().worldPtr();
  assert(world_ptr);
  printStateOn(stream,stateOf(*world_ptr));
}


void MainWindow::setFileAccessorPtr(FileAccessor *arg)
{
  _file_accessor_ptr = arg;
}
