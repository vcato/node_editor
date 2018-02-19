#include "wrapperutil.hpp"


using std::cerr;
using std::vector;
using std::string;
using std::ostream;


static void
  printOperations(ostream &stream,const Wrapper &wrapper,const TreePath &path)
{
  visitSubWrapper(
    wrapper,
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
  executeOperation(
    const Wrapper &sub_wrapper,
    const TreePath &path,
    const string &operation_name,
    Wrapper::OperationHandler &operation_handler
  )
{
  vector<string> operation_names = sub_wrapper.operationNames();
  int n_operations = operation_names.size();

  for (int index = 0; index!=n_operations; ++index) {
    if (operation_names[index]==operation_name) {
      sub_wrapper.executeOperation(index,path,operation_handler);
      return;
    }
  }

  cerr << "Could not find " << operation_name << " operation\n";
  printOperations(cerr,sub_wrapper,path);
  assert(false);
}


void
  executeOperation2(
    const Wrapper &world_wrapper,
    const TreePath &scene_path,
    const string &operation_name,
    Wrapper::OperationHandler &handler
  )
{
  visitSubWrapper(
    world_wrapper,
    scene_path,
    [&](const Wrapper &sub_wrapper){
      executeOperation(sub_wrapper,scene_path,operation_name,handler);
    }
  );
}


static const char *addBodyOperationName()
{
  return "Add Body";
}


void
  executeAddBodyFunction(
    const Wrapper &sub_wrapper,
    const TreePath &path,
    Wrapper::OperationHandler &operation_handler
  )
{
  executeOperation(
    sub_wrapper,
    path,
    addBodyOperationName(),
    operation_handler
  );
}


void
  executeAddBodyFunction2(
    const Wrapper &world_wrapper,
    const TreePath &scene_path,
    Wrapper::OperationHandler &handler
  )
{
  executeOperation2(
    world_wrapper,
    scene_path,
    addBodyOperationName(),
    handler
  );
}
