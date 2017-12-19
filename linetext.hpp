#include <string>
#include "stringutil.hpp"


extern bool lineTextHasOutput(const std::string &text);
extern int lineTextInputCount(const std::string &text);
extern float lineTextValue(const std::string &line_text);
extern float
  lineTextValue(const std::string &line_text,std::ostream &,float input_value);
