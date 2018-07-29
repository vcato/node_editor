#include <iostream>


struct StreamParser {
  std::istream &stream;
  std::string word;
  std::string error;

  StreamParser(std::istream &stream_arg) : stream(stream_arg) { }

  void setError(const std::string &arg) { error = arg; }

  void scanWord()
  {
    stream >> word;
  }

  bool hadError() const
  {
    return !error.empty();
  }

  void scanEndOfLine()
  {
    for (;;) {
      int c = stream.get();
      if (c==EOF || c=='\n') break;
    }
  }

  void skipWhitespace()
  {
    while (stream.peek()==' ') {
      stream.get();
    }
  }
};
