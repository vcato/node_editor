#ifndef ANY_HPP_
#define ANY_HPP_

#include <cassert>
#include <vector>
#include <iostream>
#include "printonany.hpp"



class Any {
  public:
    enum Type {
      void_type,
      float_type,
      vector_type
    };

    Any()
    : _type(void_type)
    {
      new (&_value.void_value)Void();
    }

    Any(float arg)
    : _type(float_type)
    {
      new (&_value.float_value)float(arg);
    }

    Any(std::vector<Any> &&arg)
    : _type(vector_type)
    {
      new (&_value.vector_value)std::vector<Any>(std::move(arg));
    }

    explicit Any(const Any& arg)
    {
      _create(arg);
    }

    Any(Any&& arg)
    {
      _create(std::move(arg));
    }

    ~Any()
    {
      _destroy();
    }

  private:
    template <typename V>
    static auto visit(Type t,const V& v)
    {
      switch (t) {
        case float_type:  return v(&Value::float_value);
        case void_type:   return v(&Value::void_value);
        case vector_type: return v(&Value::vector_value);
      }

      assert(false);
    }

  public:
    Any& operator=(Any&& arg)
    {
      if (_type!=arg._type) {
        _destroy();
        _create(std::move(arg));
      }
      else {
        visit(_type,MemberMoveAssignment{_value,std::move(arg._value)});
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
        visit(_type,MemberCopyAssignment{_value,arg._value});
      }

      return *this;
    }

    bool operator==(const Any &arg) const
    {
      if (_type!=arg._type) return false;

      return visit(_type,MemberEquality{_value,arg._value});
    }

    bool operator!=(const Any &arg) const
    {
      return !operator==(arg);
    }

    bool isVoid() const { return _type==void_type; }
    bool isVector() const { return _type==vector_type; }
    bool isFloat() const { return _type==float_type; }

    const std::vector<Any> &asVector() const
    {
      assert(_type==vector_type);
      return _value.vector_value;
    }

    float asFloat() const
    {
      assert(_type==float_type);
      return _value.float_value;
    }

    Type type() const { return _type; }

  private:
    struct Void {
      bool operator==(const Void &) const { return true; }
    };

    union Value {
      Void void_value;
      float float_value;
      std::vector<Any> vector_value;

      Value() {}
      ~Value() {}
    };

    struct MemberCopyAssignment {
      Value &a;
      const Value &b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        (a.*p) = (b.*p);
      }
    };

    struct MemberMoveAssignment {
      Value &a;
      Value &&b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        (a.*p) = std::move(b.*p);
      }
    };

    struct MemberCopy {
      Value &a;
      const Value &b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        new (&(a.*p))T(b.*p);
      }
    };

    struct MemberMove {
      Value &a;
      Value &&b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        new (&(a.*p))T(std::move(b.*p));
      }
    };

    struct MemberEquality {
      const Value &a,&b;

      template <typename T>
      bool operator()(T Value::*p) const
      {
        return (a.*p) == (b.*p);
      }
    };

    struct MemberDestroy {
      Value &_value;

      template <typename T>
      void operator()(T Value::*p) const
      {
        (_value.*p).~T();
      }
    };

    void _create(Any&& arg)
    {
      _type = arg._type;
      visit(_type,MemberMove{_value,std::move(arg._value)});
    }

    void _create(const Any& arg)
    {
      _type = arg._type;
      visit(_type,MemberCopy{_value,arg._value});
    }

  private:
    void _destroy()
    {
      visit(_type,MemberDestroy{_value});
    }

    Type _type;
    Value _value;
};


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
      printOn(stream,arg.asFloat());
      break;
    case Any::vector_type:
      printOn(stream,arg.asVector());
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
