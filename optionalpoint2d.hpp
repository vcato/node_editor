#include <cassert>
#include "point2d.hpp"

class OptionalPoint2D {
  public:
    explicit operator bool() const
    {
      return _has_value;
    }

    Point2D &operator*()
    {
      assert(_has_value);
      return _point2d;
    }

    OptionalPoint2D()
    : _has_value(false)
    {
    }

    OptionalPoint2D(const Point2D &p)
    : _point2d(p),
      _has_value(true)
    {
    }

    ~OptionalPoint2D()
    {
      if (_has_value) {
        _point2d.~Point2D();
      }
    }

  private:
    union {
      Point2D _point2d;
    };

    bool _has_value;
};
