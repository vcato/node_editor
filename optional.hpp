#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <cassert>
#include <utility>
#include <iostream>
#include "createobject.hpp"


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

    const T &operator*() const
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
        createObject(_value,std::move(arg._value));
        arg.reset();
      }
    }

    ~Optional()
    {
      reset();
    }

    Optional &operator=(const Optional &arg)
    {
      if (this==&arg) return *this;

      if (_has_value && arg._has_value) {
        assert(false);
      }
      else if (_has_value) {
        assert(false);
      }
      else if (arg._has_value) {
        createObject(_value,arg._value);
        _has_value = true;
      }
      else {
        // We don't have a value and the arg doesn't have a value, so
        // there's nothing to do.
      }

      return *this;
    }

    Optional &operator=(const T &arg)
    {
      if (_has_value) {
        _value = arg;
      }
      else {
        createObject(_value,arg);
        _has_value = true;
      }

      return *this;
    }

    bool hasValue() const { return _has_value; }

    void reset()
    {
      if (_has_value) {
        _value.~T();
        _has_value = false;
      }
    }

  private:
    union {
      T _value;
    };

    bool _has_value;
};


template <typename T>
inline std::ostream& operator<<(std::ostream &stream,const Optional<T> &arg)
{
  if (!arg) {
    stream << "nullopt";
  }
  else {
    stream << *arg;
  }

  return stream;
}


#endif /* OPTIONAL_HPP */
