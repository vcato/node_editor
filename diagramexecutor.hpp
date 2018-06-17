struct DiagramExecutor : Executor {
  Any return_value;
  std::ostream &show_stream;

  DiagramExecutor(std::ostream &show_stream_arg)
  : show_stream(show_stream_arg)
  {
  }

  virtual void executeShow(const Any& arg)
  {
    show_stream << arg << "\n";
  }

  virtual void executeReturn(const Any& arg)
  {
    return_value = arg;
  }
};
