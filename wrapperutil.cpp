#include "wrapperutil.hpp"


using std::cerr;
using std::vector;
using std::string;
using std::ostream;


static void
  printOperations(ostream &stream,const Wrapper &wrapper,const TreePath &path)
{
  wrapper.visitWrapper(
    path,
    [&](const Wrapper &sub_wrapper){
      stream << sub_wrapper.label() << " operations:\n";
      for (auto &operation_name : sub_wrapper.operationNames()) {
        stream << operation_name << "\n";
      }
    }
  );
}


void
  executeAddBodyFunction(
    const Wrapper &sub_wrapper,
    const TreePath &path,
    Wrapper::OperationHandler &operation_handler
  )
{
  vector<string> operation_names = sub_wrapper.operationNames();
  int n_operations = operation_names.size();

  for (int index = 0; index!=n_operations; ++index) {
    if (operation_names[index]=="Add Body") {
      sub_wrapper.executeOperation(index,path,operation_handler);
      return;
    }
  }

  cerr << "Could not find Add Body operation\n";
  printOperations(cerr,sub_wrapper,path);
  assert(false);
}


void
  executeAddBodyFunction2(
    const Wrapper &world_wrapper,
    const TreePath &scene_path,
    Wrapper::OperationHandler &handler
  )
{
  world_wrapper.visitWrapper(
    scene_path,
    [&](const Wrapper &sub_wrapper){
      executeAddBodyFunction(sub_wrapper,scene_path,handler);
    }
  );
}
