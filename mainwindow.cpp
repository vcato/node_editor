#include "mainwindow.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include "wrapperstate.hpp"
#include "optional.hpp"
#include "wrapper.hpp"

using std::cerr;
using std::string;


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

  std::ifstream stream(path);

  if (!stream) {
    _showError("Unable to open "+path);
    return;
  }

  ScanStateResult scan_result = scanStateFrom(stream);

  if (scan_result.isError()) {
    _showError("Unable to read "+path+"\n"+scan_result.asError().message);
    return;
  }

  treeEditor().setWorldState(scan_result.state());
}


void MainWindow::_saveProjectPressed()
{
  string path = _askForSavePath();

  std::ofstream stream(path);

  if (!stream) {
    return;
  }

  Wrapper *world_ptr = treeEditor().worldPtr();
  assert(world_ptr);
  printStateOn(stream,stateOf(*world_ptr));
}
