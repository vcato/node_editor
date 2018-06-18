#ifndef ANY_HPP_
#define ANY_HPP_

#include <cassert>
#include <vector>
#include <iostream>
#include "printonany.hpp"


class Any {
  private:
    union Value {
      float float_value;
      std::vector<Any> vector_value;

      Value() {}
      ~Value() {}
    };

  public:
    enum Type {
      void_type,
      float_type,
      vector_type
    };

    Any()
      : _type(void_type)
    {
    }

    explicit Any(const Any& arg)
    {
      _create(arg);
    }

    Any(Any&& arg)
    {
      _create(std::move(arg));
    }

    Any& operator=(Any&& arg)
    {
      if (_type!=arg._type) {
        _destroy();
        _create(std::move(arg));
      }
      else {
        switch (_type) {
          case void_type:
            break;
          case float_type:
            _value.float_value = std::move(arg._value.float_value);
            break;
          case vector_type:
            _value.vector_value = std::move(arg._value.vector_value);
            break;
        }
      }

      return *this;
    }

    Any& operator=(const Any &arg)
    {
      if (_type != arg._type) {
        _destroy();
        _create(arg);
      }
      else {
        switch (_type) {
          case void_type:
            break;
          case float_type:
            _value.float_value = arg._value.float_value;
            break;
          case vector_type:
            _value.vector_value = arg._value.vector_value;
            break;
        }
      }

      return *this;
    }

    bool operator==(const Any &arg) const
    {
      if (_type!=arg._type) return false;

      switch (_type) {
        case void_type:
          return true;
        case float_type:
          return asFloat()==arg.asFloat();
        case vector_type:
          return asVector()==arg.asVector();
      }

      assert(false);
    }

    Any(float arg)
    {
      _create(arg);
    }

    Any(std::vector<Any> &&arg)
    {
      _create(std::move(arg));
    }

    template <typename T> friend const T& any_cast(const Any &);

    template <typename T>
      const T& as() const
      {
        return any_cast<T>(*this);
      }

    bool isVoid() const { return _type==void_type; }
    bool isVector() const { return _type==vector_type; }
    bool isFloat() const { return _type==float_type; }
    const std::vector<Any> &asVector() const { return as<std::vector<Any>>(); }
    float asFloat() const { return as<float>(); }

    Type type() const { return _type; }

    ~Any()
    {
      _destroy();
    }

  private:
    void _create(Any&& arg)
    {
      switch (arg._type) {
        case void_type:
          _create();
          break;
        case float_type:
          _create(std::move(arg._value.float_value));
          break;
        case vector_type:
          _create(std::move(arg._value.vector_value));
          break;
      }
    }

    void _create(const Any& arg)
    {
      switch (arg._type) {
        case void_type:
          _create();
          break;
        case float_type:
          _create(arg._value.float_value);
          break;
        case vector_type:
          _create(arg._value.vector_value);
          break;
      }
    }

    void _create()
    {
      _type = void_type;
    }

    void _create(float arg)
    {
      _type = float_type;
      new (&_value.float_value)float(arg);
    }

    void _create(std::vector<Any> &&arg)
    {
      _type = vector_type;
      new (&_value.vector_value)std::vector<Any>(std::move(arg));
    }

    void _create(const std::vector<Any> &arg)
    {
      _type = vector_type;
      new (&_value.vector_value)std::vector<Any>(arg);
    }

    void _destroy()
    {
      switch (_type) {
        case float_type:
          {
            using T = float;
            _value.float_value.~T();
          }
          break;
        case vector_type:
          {
            using T = std::vector<Any>;
            _value.vector_value.~T();
          }
          break;
        case void_type:
          break;
      }
    }

    Type _type;
    Value _value;
};


template<>
inline const float& any_cast<float>(const Any &arg)
{
  assert(arg._type==Any::float_type);
  return arg._value.float_value;
}

template<>
inline const std::vector<Any>& any_cast<std::vector<Any>>(const Any &arg)
{
  assert(arg._type==Any::vector_type);
  return arg._value.vector_value;
}


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
      stream << "void()";
      break;
  }
}


inline std::ostream& operator<<(std::ostream &stream,const Any &arg)
{
  printOn(stream,arg);
  return stream;
}


#endif /* ANY_HPP_ */
