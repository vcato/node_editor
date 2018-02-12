#include "wrapperutil.hpp"


using std::cerr;
using std::ostream;


static void
  printOperations(ostream &stream,const Wrapper &wrapper,const TreePath &path)
{
  wrapper.visitWrapper(path,[&](const Wrapper &sub_wrapper){
    stream << sub_wrapper.label() << " operations:\n";
    sub_wrapper.withOperations(
      path,
      [&](
        const Wrapper::OperationName &operation_name,
        Wrapper::PerformOperationFunction
      ){
      	stream << operation_name << "\n";
      }
    );
  });
}


Wrapper::PerformOperationFunction
  findAddBodyFunction(const Wrapper &wrapper,const TreePath &path)
{
  Wrapper::PerformOperationFunction add_body_function;

  Wrapper::OperationVisitor visitor =
    [&](
      const Wrapper::OperationName &operation_name,
      Wrapper::PerformOperationFunction perform_operation_function
    ) {
      if (operation_name=="Add Body") {
      	add_body_function = perform_operation_function;
      }
    };

  wrapper.withOperations(path,visitor);

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
