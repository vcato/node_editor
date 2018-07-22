#ifndef ANY_HPP_
#define ANY_HPP_

#include <cassert>
#include <vector>
#include <iostream>
#include "printonany.hpp"
#include "basicvariant.hpp"

struct AnyPolicy;

using Any = BasicVariant<AnyPolicy>;

struct AnyPolicy {
  public:
    enum Type {
      void_type,
      float_type,
      vector_type
    };

    struct Void {
      bool operator==(const Void &) const { return true; }
    };

    AnyPolicy(VariantPolicyNoInitTag)
    {
    }

    AnyPolicy()
    : _type(void_type)
    {
      new (&_value.void_value)Void();
    }

    AnyPolicy(float arg)
    : _type(float_type)
    {
      new (&_value.float_value)float(arg);
    }

    AnyPolicy(std::vector<Any> &&arg)
    : _type(vector_type)
    {
      new (&_value.vector_value)std::vector<Any>(std::move(arg));
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

    Void asVoid() const
    {
      assert(_type==void_type);
      return _value.void_value;
    }

  protected:
    union Value {
      Void void_value;
      float float_value;
      std::vector<Any> vector_value;

      Value() {}
      ~Value() {}
    };

    template <typename V>
    static auto withMemberPtrFor(Type t,const V& v)
    {
      switch (t) {
        case float_type:  return v(&Value::float_value);
        case void_type:   return v(&Value::void_value);
        case vector_type: return v(&Value::vector_value);
      }

      assert(false);
    }

    Type _type;
    Value _value;
};


template <>
inline void printOn(std::ostream &stream,const float &arg)
{
  stream << arg;
}

template <>
inline void printOn(std::ostream &stream,const Any::Void &)
{
  stream << "void()";
}


template <>
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


inline std::ostream& operator<<(std::ostream &stream,const Any &arg)
{
  printOn(stream,arg);
  return stream;
}


#endif /* ANY_HPP_ */
