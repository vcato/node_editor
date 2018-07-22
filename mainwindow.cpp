#include "mainwindow.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include "wrapperstate.hpp"
#include "optional.hpp"

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
    assert(false);
  }

  std::ifstream stream(path);

  if (!stream) {
    assert(false);
  }

#if 0
  Optional<WrapperState> maybe_state = scanStateFrom(stream);

  if (!maybe_state) {
    assert(false);
  }

  Wrapper *world_ptr = treeEditor().worldPtr();

  assert(world_ptr);

  world_ptr->setState(*maybe_state);
#endif
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
