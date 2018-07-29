#ifndef LINETEXT_HPP
#define LINETEXT_HPP

#include <string>
#include <vector>
#include <iosfwd>
#include <cassert>
#include "stringutil.hpp"
#include "executor.hpp"
#include "optional.hpp"


extern bool lineTextHasOutput(const std::string &text);
extern int lineTextInputCount(const std::string &text);


extern Optional<Any>
  evaluateLineText(
    const std::string &line_text_arg,
    const std::vector<Any> &input_values,
    Executor &executor,
    std::ostream &error_stream
  );

#endif /* LINETEXT_HPP */
