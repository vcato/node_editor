#include "wrapper.hpp"


extern void
  executeAddBodyFunction2(
    const Wrapper &world_wrapper,
    const TreePath &scene_path,
    Wrapper::TreeObserver &
  );

extern void
  executeAddBodyFunction(
    const Wrapper &sub_wrapper,
    const TreePath &path,
    Wrapper::TreeObserver &
  );

// Execute the operation on a particular wrapper.
extern void
  executeOperation(
    const Wrapper &sub_wrapper,
    const TreePath &,
    const std::string &operation_name,
    Wrapper::TreeObserver &
  );

// Execute the operation on a sub-wrapper of the wrapper.
extern void
  executeOperation2(
    const Wrapper &world_wrapper,
    const TreePath &,
    const std::string &operation_name,
    Wrapper::TreeObserver &
  );

extern void
  executeOperation(
    const Wrapper &wrapper,
    const std::string &path_string,
    const std::string &operation_name,
    Wrapper::TreeObserver &
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
