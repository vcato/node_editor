#include <map>


struct DiagramExecutor : Executor {
  Any return_value;
  std::ostream &show_stream;
  std::map<std::string,Any> environment;

  DiagramExecutor(std::ostream &show_stream_arg)
  : show_stream(show_stream_arg)
  {
  }

  void executeShow(const Any& arg) override
  {
    show_stream << arg << "\n";
  }

  void executeReturn(const Any& arg) override
  {
    return_value = arg;
  }

  Optional<Any> variableValue(const std::string &name) const override
  {
    auto iter = environment.find(name);

    if (iter==environment.end()) {
      return {};
    }

    return iter->second;
  }
};
