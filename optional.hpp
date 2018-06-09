#include <cassert>


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

    Optional()
    : _has_value(false)
    {
    }

    Optional(const T &arg)
    : _value(arg),
      _has_value(true)
    {
    }

    ~Optional()
    {
      if (_has_value) {
        _value.~T();
      }
    }

  private:
    union {
      T _value;
    };

    bool _has_value;
};
