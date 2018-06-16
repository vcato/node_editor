#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP


#include <cassert>
#include <utility>


template <typename T>
class Optional {
  public:
    explicit operator bool() const
    {
      return _has_value;
    }

    T &operator*()
    {
      assert(_has_value);
      return _value;
    }

    T *operator->()
    {
      assert(_has_value);
      return &_value;
    }

    Optional()
    : _has_value(false)
    {
    }

    Optional(const T &arg)
    : _value(arg),
      _has_value(true)
    {
    }

    Optional(T&& arg)
    : _value(std::move(arg)),
      _has_value(true)
    {
    }

    Optional(Optional&& arg)
    : _has_value(arg._has_value)
    {
      if (arg._has_value) {
        new (&_value)T(std::move(arg._value));
        arg._has_value = false;
      }
    }

    ~Optional()
    {
      if (_has_value) {
        _value.~T();
      }
    }

    Optional &operator=(const T &arg)
    {
      if (_has_value) {
        _value = arg;
      }
      else {
        new (&_value)T(arg);
        _has_value = true;
      }

      return *this;
    }

    bool hasValue() const { return _has_value; }

  private:
    union {
      T _value;
    };

    bool _has_value;
};


#endif /* OPTIONAL_HPP */
