#include "wrapperstate.hpp"
#include "diagram.hpp"


extern WrapperState makeDiagramWrapperState(const Diagram &diagram);

extern bool
  buildDiagramFromState(
    Diagram &diagram,
    const WrapperState &state,
    std::string &error
  );

extern Diagram makeDiagramFromWrapperState(const WrapperState &state);
