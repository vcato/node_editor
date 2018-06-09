#ifndef LINETEXT_HPP
#define LINETEXT_HPP

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include "stringutil.hpp"


extern bool lineTextHasOutput(const std::string &text);
extern int lineTextInputCount(const std::string &text);


struct Executor {
  virtual void executeShow(float) = 0;
  virtual void executeReturn(float) = 0;
};


struct StreamExecutor : Executor {
  std::ostream &stream;

  StreamExecutor(std::ostream &stream_arg)
  : stream(stream_arg)
  {
  }

  void executeShow(float value)
  {
    stream << value << "\n";
  }

  virtual void executeReturn(float)
  {
    assert(false);
  }
};


extern float
  evaluateLineText(
    const std::string &line_text,
    const std::vector<float> &input_values,
    Executor &executor
  );

#endif /* LINETEXT_HPP */
