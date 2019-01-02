#ifndef DIAGRAMEXECUTIONCONTEXT_HPP_
#define DIAGRAMEXECUTIONCONTEXT_HPP_

#include "environment.hpp"


struct DiagramExecutionContext {
  std::ostream &show_stream;
  std::ostream &error_stream;
  Environment *parent_environment_ptr = nullptr;
};


#endif /* DIAGRAMEXECUTIONCONTEXT_HPP_ */
