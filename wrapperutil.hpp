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
