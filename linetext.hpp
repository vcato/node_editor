#ifndef LINETEXT_HPP
#define LINETEXT_HPP

#include <string>
#include <vector>
#include "stringutil.hpp"


extern bool lineTextHasOutput(const std::string &text);
extern int lineTextInputCount(const std::string &text);
extern float lineTextValue(const std::string &line_text);

extern float
  lineTextValue(const std::string &line_text,std::ostream &,float input_value);

extern float
  lineTextValue(
    const std::string &line_text,
    std::ostream &,
    const std::vector<float> &input_values
  );

struct Executor {
  virtual void executeShow(float) = 0;
  virtual void executeReturn(float) = 0;
  virtual void output(float) = 0;
};

extern void
  evaluateLineText(
    const std::string &line_text,
    const std::vector<float> &input_values,
    Executor &executor
  );

#endif /* LINETEXT_HPP */
