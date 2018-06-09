#ifndef LINETEXT_HPP
#define LINETEXT_HPP

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include "stringutil.hpp"
#include "executor.hpp"


extern bool lineTextHasOutput(const std::string &text);
extern int lineTextInputCount(const std::string &text);


extern float
  evaluateLineText(
    const std::string &line_text,
    const std::vector<float> &input_values,
    Executor &executor
  );

#endif /* LINETEXT_HPP */
