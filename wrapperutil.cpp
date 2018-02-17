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


Wrapper::PerformOperationFunction
  findAddBodyFunction(const Wrapper &wrapper,const TreePath &path)
{
  Wrapper::PerformOperationFunction add_body_function;

  vector<string> operation_names = wrapper.operationNames();
  int n_operations = operation_names.size();

  for (
    int operation_index = 0;
    operation_index!=n_operations;
    ++operation_index
  ) {
    if (operation_names[operation_index]=="Add Body") {
      add_body_function = wrapper.operationFunction(operation_index,path);
      break;
    }
  }

  if (!add_body_function) {
    cerr << "Could not find Add Body operation\n";
    printOperations(cerr,wrapper,path);
  }

  assert(add_body_function);
  return add_body_function;

}


Wrapper::PerformOperationFunction
  findAddBodyFunction2(const Wrapper &wrapper,const TreePath &path)
{
  Wrapper::PerformOperationFunction result;

  wrapper.visitWrapper(path,[&](const Wrapper &sub_wrapper){
    result = findAddBodyFunction(sub_wrapper,path);
  });

  return result;
}
