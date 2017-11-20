#include <string>
#include "stringutil.hpp"


extern bool lineTextHasInput(const std::string &text);
extern bool lineTextHasOutput(const std::string &text);
extern float lineTextValue(const std::string &line_text);
extern float
  lineTextValue(const std::string &line_text,std::ostream &,float input_value);
