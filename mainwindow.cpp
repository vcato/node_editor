#include "mainwindow.hpp"

#include <iostream>
#include <fstream>
#include <string>

using std::cerr;
using std::string;


void MainWindow::setWorldPtr(Wrapper *world_ptr_arg)
{
  treeEditor().setWorldPtr(world_ptr_arg);
}


void MainWindow::_saveProjectPressed()
{
  string path = _askForSavePath();

  std::ofstream stream(path);

  if (!stream) {
    assert(false);
  }

  Wrapper *world_ptr = treeEditor().worldPtr();
  assert(world_ptr);
  cerr << "printWrapperOn() not implemented\n";
  // printWrapperOn(*world_ptr,stream);
}
