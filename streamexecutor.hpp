#include "executor.hpp"


inline void printOn(std::ostream &stream,const Any &arg);


inline void printOn(std::ostream &stream,float arg)
{
  stream << arg;
}

inline void printOn(std::ostream &stream,const std::vector<Any> &arg)
{
  stream << "[";

  auto iter = arg.begin();

  if (iter!=arg.end()) {
    printOn(stream,*iter);
    ++iter;
  }

  while (iter!=arg.end()) {
    stream << ",";
    printOn(stream,*iter);
    ++iter;
  }

  stream << "]";
}


inline void printOn(std::ostream &stream,const Any &arg)
{
  switch (arg.type()) {
    case Any::float_type:
      printOn(stream,arg.as<float>());
      break;
    case Any::vector_type:
      printOn(stream,arg.as<std::vector<Any>>());
      break;
    case Any::void_type:
      assert(false);
      break;
  }
}


struct StreamExecutor : Executor {
  std::ostream &stream;

  StreamExecutor(std::ostream &stream_arg)
  : stream(stream_arg)
  {
  }

  void print(float value)
  {
    stream << value;
  }

  void executeShow(float value)
  {
    print(value);
    stream << "\n";
  }

  void print(const std::vector<Any> &value)
  {
    printOn(stream,value);
  }

  void print(const Any &value)
  {
    printOn(stream,value);
  }

  void executeShow(const std::vector<Any> &value)
  {
    print(value);
    stream << "\n";
  }

  virtual void executeReturn(float)
  {
    assert(false);
  }

  virtual void executeReturn(const std::vector<Any> &)
  {
    assert(false);
  }
};
