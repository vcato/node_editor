struct StreamParser {
  std::istream &stream;
  std::string word;
  std::string error;

  StreamParser(std::istream &stream_arg) : stream(stream_arg) { }

  void scanWord()
  {
    stream >> word;
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
