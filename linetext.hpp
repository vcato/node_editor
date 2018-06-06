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
  virtual void output(float) = 0;
};


struct StreamExecutor : Executor {
  std::ostream &stream;
  float output_value = 0;

  StreamExecutor(std::ostream &stream_arg)
  : stream(stream_arg)
  {
  }

  virtual void output(float arg)
  {
    output_value = arg;
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


extern void
  evaluateLineText(
    const std::string &line_text,
    const std::vector<float> &input_values,
    Executor &executor
  );

#endif /* LINETEXT_HPP */
