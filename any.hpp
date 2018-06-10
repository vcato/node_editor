#ifndef ANY_HPP_
#define ANY_HPP_

#include <cassert>


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

    Any(const Any&) = delete;

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

#endif /* ANY_HPP_ */
