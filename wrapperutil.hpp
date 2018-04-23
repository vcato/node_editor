#include "wrapper.hpp"


extern void
  executeAddBodyFunction2(
    const Wrapper &world_wrapper,
    const TreePath &scene_path,
    Wrapper::OperationHandler &handler
  );

extern void
  executeAddBodyFunction(
    const Wrapper &sub_wrapper,
    const TreePath &path,
    Wrapper::OperationHandler &operation_handler
  );

// Execute the operation on a particular wrapper.
extern void
  executeOperation(
    const Wrapper &sub_wrapper,
    const TreePath &,
    const std::string &operation_name,
    Wrapper::OperationHandler &operation_handler
  );

// Execute the operation on a sub-wrapper of the wrapper.
extern void
  executeOperation2(
    const Wrapper &world_wrapper,
    const TreePath &,
    const std::string &operation_name,
    Wrapper::OperationHandler &handler
  );

extern void
  executeOperation(
    const Wrapper &wrapper,
    const std::string &path_string,
    const std::string &operation_name,
    Wrapper::OperationHandler &operation_handler
  );

extern int
  operationIndex(
    const Wrapper &sub_wrapper,
    const std::string &operation_name
  );

extern int
  operationIndex2(
    const Wrapper &world_wrapper,
    const TreePath &path,
    const std::string &operation_name
  );

extern Diagram *diagramPtr(const Wrapper &wrapper,const TreePath &path);


extern TreePath
  makePath(const Wrapper &wrapper,const std::string &path_string);
